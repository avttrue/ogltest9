#include "engineobject3d.h"

#include <QDir>

EngineObject3D::~EngineObject3D()
{
    for(auto o: m_Objects) delete o;
}

bool EngineObject3D::load(const QString &filename)
{
    QFile objfile(filename);

    if(! objfile.exists()) { qCritical() << "File not exist:" << filename; return false; }
    if(! objfile.open(QFile::ReadOnly))  { qCritical() << "File not opened:" << filename; return false; }

    QTextStream input(&objfile);
    QVector<QVector3D> coords;
    QVector<QVector2D> texturcoords;
    QVector<QVector3D> normals;

    QVector<VertexData> vertexes;
    QVector<GLuint> indexes;
    QString mtlName;
    Object3D* object = nullptr;

    qDebug() << "Reading" << filename << "...";

    bool ok = true;
    while(!input.atEnd() && ok)
    {
        auto str = input.readLine(); if(str.isEmpty()) continue;
        auto strlist = str.split(' '); strlist.removeAll("");
        auto key = strlist.at(0);

        if (key == "#") { qDebug() << str; }
        else if(key == "mtllib")
        {
            if(strlist.size() > 1)
            {
                auto file = QFileInfo(filename).absolutePath() + QDir::separator() + strlist.at(1);
                if(QFile(file).exists()) ok = m_MatLibrary.load(file);
                else { qCritical() << "File not exists:" << file; ok = false; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key.toLower() == "o")
        {
            qDebug() << str;
        }
        else if(key.toLower() == "g")
        {
            qDebug() << str;
        }
        else if(key.toLower() == "s")
        {
            qDebug() << str;
        }
        else if(key.toLower() == "v")
        {
            if(strlist.size() > 3)
            {
                coords.append(QVector3D(strlist.at(1).toFloat(&ok),
                                        strlist.at(2).toFloat(&ok),
                                        strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key.toLower() == "vt")
        {
            if(strlist.size() > 2)
            {
                texturcoords.append(QVector2D(strlist.at(1).toFloat(&ok),
                                              strlist.at(2).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key.toLower() == "vn")
        {
            if(strlist.size() == 4)
            {
                normals.append(QVector3D(strlist.at(1).toFloat(&ok),
                                         strlist.at(2).toFloat(&ok),
                                         strlist.at(3).toFloat(&ok)));
                if(!ok) { qCritical() << "Error at line (format):" << str; }
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
        else if(key.toLower() == "f")
        {
            for(int i = 1; i < strlist.size(); i++)
            {
                auto v = strlist.at(i).split('/');
                if(v.size() == 3 && !v.at(1).isEmpty() && !v.at(2).isEmpty())
                {
                    vertexes.append(VertexData(coords.at(v.at(0).toInt(&ok, 10) - 1),
                                               texturcoords.at(v.at(1).toInt(&ok, 10) - 1),
                                               normals.at(v.at(2).toInt(&ok, 10) - 1)));
                    indexes.append(static_cast<GLuint>(indexes.size()));
                }
                else
                {
                    qCritical() << "Unsupported OBJ data format:" << strlist.at(i);
                    ok = false; break;
                }
            }
            if(!ok) { qCritical() << "Error at line (format):" << str; }
        }
        else if(key.toLower() == "usemtl")
        {
            if(strlist.size() > 1)
            {
                if(object) object->init(vertexes, indexes, m_MatLibrary.get(mtlName));
                mtlName = strlist.at(1);
                add(object);

                object = new Object3D;
                vertexes.clear();
                indexes.clear();
            }
            else { qCritical() << "Error at line (count):" << str; ok = false; }
        }
    }

    objfile.close(); qDebug() <<  "... done" << filename;

    if(!ok) return false;

    qDebug() << "Object is loaded successfully";
    if(object) object->init(vertexes, indexes, m_MatLibrary.get(mtlName));
    add(object);

    return true;
}

void EngineObject3D::add(Object3D *obj)
{
    if(! obj) return;
    for(auto o: m_Objects) if (o == obj) return;
    m_Objects.append(obj);
}

Object3D *EngineObject3D::get(int index)
{
    if(index > -1 && index < m_Objects.size()) return m_Objects.at(index);
    return nullptr;
}

void EngineObject3D::rotate(const QQuaternion &r)
{
    for(auto o: m_Objects) o->rotate(r);
}

void EngineObject3D::translate(const QVector3D &t)
{
    for(auto o: m_Objects) o->translate(t);
}

void EngineObject3D::scale(const float &s)
{
    for(auto o: m_Objects) o->scale(s);
}

void EngineObject3D::setGlobalTransform(const QMatrix4x4 &gt)
{
    for(auto o: m_Objects) o->setGlobalTransform(gt);
}

void EngineObject3D::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    for(auto o: m_Objects) o->draw(program, functions);
}
