#include "oglwidget.h"
#include "engineobject3d.h"
#include "groupobjects.h"
#include "eye.h"
#include "skybox.h"
#include "material.h"

#include <QMouseEvent>
#include <QOpenGLContext>
#include <QtMath>

OGLWidget::OGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_GlobalGroup = new GroupObjects("Global");
    m_Eye = new Eye;
    m_Eye->translate(QVector3D(0.0f, 0.0f, -5.0f));
}

OGLWidget::~OGLWidget()
{
    makeCurrent(); // убирает варнинг "QOpenGLTexturePrivate::destroy() called without a current context"
    for(auto o: m_Objects) delete o;
    for(auto o: m_Groups) delete o;
    delete m_Eye;
    delete m_SkyBox;
    delete m_GlobalGroup;
}

void OGLWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    initShaders();

    float step = 2.0f;
    m_Groups.append(new GroupObjects("cube3"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initCube(1.0f, ":/textures/cube3.png");
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(-4.0f, 0.0f, 0.0f));

    m_Groups.append(new GroupObjects("cube1"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initCube(1.0f, ":/textures/cube1.png");
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(4.0f, 0.0f, 0.0f));

    m_Groups.append(new GroupObjects("cube2"));
    for(float x = -step; x <= step; x += step)
    {
        for(float y = -step; y <= step; y += step)
        {
            for(float z = -step; z <= step; z += step)
            {
                initCube(1.0f, ":/textures/cube2.png");
                m_Objects.last()->translate(QVector3D(x, y, z));
                m_Groups.last()->add(m_Objects.last());
            }
        }
    }
    m_Groups.last()->translate(QVector3D(0.0f, 0.0f, -8.0f));

    m_Groups.append(new GroupObjects("sphere-cube-pyramid"));
    m_Objects.append(new EngineObject3D);
    if(m_Objects.last()->load(":/models/sphere.obj"))
    {
        m_Objects.last()->translate(QVector3D(-2.0f, 2.0f, 2.0f));
        m_Groups.last()->add(m_Objects.last());

    }
    m_Objects.append(new EngineObject3D);
    if(m_Objects.last()->load(":/models/cube.obj"))
    {
        m_Objects.last()->translate(QVector3D(2.0f, -2.0f, 2.0f));
        m_Groups.last()->add(m_Objects.last());
    }
    m_Objects.append(new EngineObject3D);
    if(m_Objects.last()->load(":/models/pyramid.obj"))
    {
        m_Objects.last()->translate(QVector3D(2.0f, 2.0f, -2.0f));
        m_Groups.last()->add(m_Objects.last());
    }
    m_Groups.last()->translate(QVector3D(10.0f, -10.0f, -10.0f));

    m_Groups.append(new GroupObjects("All sphere-cube-pyramid + cubes"));
    for(int i = 0; i < m_Groups.size() -1; i++)
        m_Groups.last()->add(m_Groups.at(i));

    m_GlobalGroup->add(m_Groups.last());

    m_SkyBox = new SkyBox(1000.0, QImage(":/textures/skybox1.jpg"));

    m_Timer.start(30, this);
}

void OGLWidget::resizeGL(int w, int h)
{
    float aspect = w / (h? static_cast<float>(h) : 1);
    m_PojectionMatrix.setToIdentity();
    m_PojectionMatrix.perspective(45, aspect, 0.01f, 1000.0f); // посл. 2 параметра - настройка плоскостей отсечения
}

void OGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_ProgramSkyBox.bind();
    m_ProgramSkyBox.setUniformValue("u_ProjectionMatrix", m_PojectionMatrix);
    m_Eye->draw(&m_ProgramSkyBox);
    m_SkyBox->draw(&m_ProgramSkyBox, context()->functions());
    m_ProgramSkyBox.release();

    m_Program.bind();
    m_Program.setUniformValue("u_ProjectionMatrix", m_PojectionMatrix);
    m_Program.setUniformValue("u_LightPosition", QVector4D(0.0f, 0.0f, 0.0f, 1.0f));
    m_Program.setUniformValue("u_LightPower", 1.0f);
    m_Eye->draw(&m_Program);
    m_GlobalGroup->draw(&m_Program, context()->functions());
    m_Program.release();
}

void OGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        m_MousePosition = QVector2D(event->localPos());
    }

    event->accept();
}

void OGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        QVector2D diffpos = QVector2D(event->localPos()) - m_MousePosition;
        m_MousePosition = QVector2D(event->localPos());

        float angle = diffpos.length() / 2.0f;
        QVector3D axis = QVector3D(diffpos.y(), diffpos.x(), 0.0f);
        m_Eye->rotate(QQuaternion::fromAxisAndAngle(axis, angle));

        update();
    }

    event->accept();
}

void OGLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0) m_Eye->translate(QVector3D(0.0f, 0.0f, 0.25f));
    else if(event->delta() < 0) m_Eye->translate(QVector3D(0.0f, 0.0f, -0.25f));

    update();
}

void OGLWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Tab:
        currentGroup >= m_Groups.size() - 1 ? currentGroup = 0 : currentGroup++;
        for(auto g: m_Groups) g->del(m_Eye);
        m_Groups.at(currentGroup)->add(m_Eye);
        break;
    case Qt::Key_Delete:
        for(auto g: m_Groups) g->del(m_Eye);
        break;
    case Qt::Key_Escape:
        for(auto g: m_Groups) g->del(m_Eye);
        QMatrix4x4 m; m.setToIdentity();
        m_Eye->setGlobalTransform(m);
        break;
    }
}

void OGLWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    for(int i = 0; i < m_Objects.count(); i++)
    {
        if(i % 2 == 0)
        {
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        1.0f, 0.0f, 0.0f, static_cast<float>(qSin(m_AngleObject))));
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        0.0f, 1.0f, 0.0f, static_cast<float>(qCos(m_AngleObject))));
        }
        else
        {
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleObject))));
            m_Objects.at(i)->rotate(QQuaternion::fromAxisAndAngle(
                                        1.0f, 0.0f, 0.0f, static_cast<float>(qCos(m_AngleObject))));
        }
    }

    m_Groups.at(0)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qSin(m_AngleGrop1))));
    m_Groups.at(0)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qSin(m_AngleGrop1))));

    m_Groups.at(1)->rotate(QQuaternion::fromAxisAndAngle(
                               1.0f, 0.0f, 0.0f, static_cast<float>(qCos(m_AngleGrop2))));
    m_Groups.at(1)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qCos(m_AngleGrop2))));

    m_Groups.at(2)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleGrop3))));
    m_Groups.at(2)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qCos(m_AngleGrop3))));

    m_Groups.at(3)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(qSin(m_AngleGrop4))));
    m_Groups.at(3)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 0.0f, 1.0f, static_cast<float>(qCos(m_AngleGrop4))));

    m_Groups.at(4)->rotate(QQuaternion::fromAxisAndAngle(
                               1.0f, 0.0f, 0.0f, static_cast<float>(-qSin(m_AngleGropMain))));
    m_Groups.at(4)->rotate(QQuaternion::fromAxisAndAngle(
                               0.0f, 1.0f, 0.0f, static_cast<float>(-qCos(m_AngleGropMain))));

    m_AngleObject += M_PI / 360.0;
    m_AngleGrop1 += M_PI / 270.0;
    m_AngleGrop2 -= M_PI / 270.0;
    m_AngleGrop3 -= M_PI / 270.0;
    m_AngleGrop4 -= M_PI / 270.0;
    m_AngleGropMain += M_PI / 720.0;

    update();
}

void OGLWidget::initShaders()
{
    if(! m_Program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.vsh"))
        close();
    if(! m_Program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.fsh"))
        close();

    if(! m_Program.link())
        close();

    if(! m_ProgramSkyBox.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox.vsh"))
        close();
    if(! m_ProgramSkyBox.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox.fsh"))
        close();

    if(! m_ProgramSkyBox.link())
        close();
}

void OGLWidget::initCube(float width, const QString &imgpath)
{
    float width_div_2 = width / 2.0f;

    QVector<VertexData> vertexes;
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, 1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(0.0f, 1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(width_div_2, width_div_2, -width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(0.0f, 0.0f, -1.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f)));

    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, width_div_2, -width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(-1.0f, 0.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f)));

    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, width_div_2),
                               QVector2D(0.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(-width_div_2, -width_div_2, -width_div_2),
                               QVector2D(0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, width_div_2),
                               QVector2D(1.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f)));
    vertexes.append(VertexData(QVector3D(width_div_2, -width_div_2, -width_div_2),
                               QVector2D(1.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f)));

    QVector<GLuint> indexes;
    for(GLuint i = 0; i < 24; i += 4)
    {
        indexes.append(i + 0); indexes.append(i + 1); indexes.append(i + 2);
        indexes.append(i + 2); indexes.append(i + 1); indexes.append(i + 3);
    }

    Material* mtl = new Material;
    mtl->setDiffuseMap(imgpath);
    mtl->setShines(96.0f);
    mtl->setDiffuseColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setAmbienceColor(QVector3D(1.0f, 1.0f, 1.0f));
    mtl->setSpecularColor(QVector3D(1.0f, 1.0f, 1.0f));

    EngineObject3D* eo3d = new EngineObject3D;
    eo3d->add(new Object3D(vertexes, indexes, mtl));
    m_Objects.append(eo3d);
}

