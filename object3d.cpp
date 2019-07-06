#include "object3d.h"
#include "material.h"

#include<QOpenGLTexture>
#include<QOpenGLShaderProgram>
#include<QOpenGLFunctions>

Object3D::Object3D()
    : m_VertexBuffer(QOpenGLBuffer::VertexBuffer), // это значение по-умолчанию
      m_IndexBuffer(QOpenGLBuffer::IndexBuffer),
      m_Texture(nullptr)
{
    m_Scale = 1.0f;
}

Object3D::Object3D(const QVector<VertexData> &vert, const QVector<GLuint> &ind, Material *mat)
    : m_VertexBuffer(QOpenGLBuffer::VertexBuffer), // это значение по-умолчанию
      m_IndexBuffer(QOpenGLBuffer::IndexBuffer),
      m_Texture(nullptr)
{
    m_Scale = 1.0f;
    init(vert, ind, mat);
}

Object3D::~Object3D()
{
    free();
}

void Object3D::free()
{
    if(m_VertexBuffer.isCreated()) m_VertexBuffer.destroy();
    if(m_IndexBuffer.isCreated()) m_IndexBuffer.destroy();
    if(m_Texture != nullptr && m_Texture->isCreated())
    { delete m_Texture; m_Texture = nullptr; }
}

void Object3D::init(const QVector<VertexData> &vert, const QVector<GLuint> &ind, Material *mat)
{
    free();

    m_VertexBuffer.create();
    m_VertexBuffer.bind();
    m_VertexBuffer.allocate(vert.constData(), vert.size() * static_cast<int>(sizeof(VertexData)));
    m_VertexBuffer.release();// temp

    m_IndexBuffer.create();
    m_IndexBuffer.bind();
    m_IndexBuffer.allocate(ind.constData(), ind.size() * static_cast<int>(sizeof(GLuint)));
    m_IndexBuffer.release();// temp

    m_Material = mat;

    m_Texture = new QOpenGLTexture(m_Material->DiffuseMap().mirrored());
    m_Texture->setMinificationFilter(QOpenGLTexture::Nearest);
    m_Texture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_Texture->setWrapMode(QOpenGLTexture::Repeat);
}

void Object3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    if(!m_VertexBuffer.isCreated() || !m_IndexBuffer.isCreated()) return;

    m_Texture->bind(0);
    program->setUniformValue("u_Texture", 0);

    QMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    modelMatrix.translate(m_Translate); // здесь важен порядок преобразований *
    modelMatrix.rotate(m_Rotate);       // *
    modelMatrix.scale(m_Scale);         // *
    modelMatrix = m_GlobalTransform * modelMatrix;

    program->setUniformValue("u_ModelMatrix", modelMatrix);
    program->setUniformValue("u_MaterialProperty.DiffuseColor", m_Material->DiffuseColor());
    program->setUniformValue("u_MaterialProperty.AmbienceColor", m_Material->AmbienceColor());
    program->setUniformValue("u_MaterialProperty.SpecularColor", m_Material->SpecularColor());
    program->setUniformValue("u_MaterialProperty.Shines",
                             //96.078431f
                             m_Material->Shines()
                             );
    program->setUniformValue("u_IsUseDiffuseMap", m_Material->isUseDiffuseMap());

    m_VertexBuffer.bind();

    int offset = 0;

    int vertloc = program->attributeLocation("a_position");
    program->enableAttributeArray(vertloc);
    program->setAttributeBuffer(vertloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    offset += sizeof(QVector3D);

    int texloc = program->attributeLocation("a_textcoord");
    program->enableAttributeArray(texloc);
    program->setAttributeBuffer(texloc, GL_FLOAT, offset, 2, sizeof(VertexData));

    offset += sizeof(QVector2D);

    int normloc = program->attributeLocation("a_normal");
    program->enableAttributeArray(normloc);
    program->setAttributeBuffer(normloc, GL_FLOAT, offset, 3, sizeof(VertexData));

    m_IndexBuffer.bind();

    functions->glDrawElements(GL_TRIANGLES, m_IndexBuffer.size(), GL_UNSIGNED_INT, nullptr);

    m_VertexBuffer.release();
    m_IndexBuffer.release();
    m_Texture->release();
}

void Object3D::rotate(const QQuaternion &r)
{
    m_Rotate *= r;
}

void Object3D::translate(const QVector3D &t)
{
    m_Translate += t;
}

void Object3D::scale(const float &s)
{
    m_Scale *= s;
}

void Object3D::setGlobalTransform(const QMatrix4x4 &gt)
{
    m_GlobalTransform = gt;
}
