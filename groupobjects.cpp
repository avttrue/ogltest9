#include "groupobjects.h"

GroupObjects::GroupObjects(const QString &name)
{
    m_Name = name;
    m_Scale = 1.0f;
}

void GroupObjects::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *functions)
{
    for(auto o: m_Objects) o->draw(program, functions);
}

void GroupObjects::rotate(const QQuaternion &r)
{
    m_Rotate = r * m_Rotate;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void GroupObjects::translate(const QVector3D &t)
{
    m_Translate += t;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void GroupObjects::scale(const float &s)
{
    m_Scale *= s;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void GroupObjects::setGlobalTransform(const QMatrix4x4 &gt)
{
    m_GlobalTransform = gt;

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    for(auto o: m_Objects) o->setGlobalTransform(localmatrix);
}

void GroupObjects::add(Transformational *obj)
{
    m_Objects.append(obj);

    QMatrix4x4 localmatrix;
    localmatrix.setToIdentity();
    localmatrix.translate(m_Translate);
    localmatrix.rotate(m_Rotate);
    localmatrix.scale(m_Scale);
    localmatrix = m_GlobalTransform * localmatrix;

    obj->setGlobalTransform(localmatrix);
    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
}

bool GroupObjects::del(Transformational *obj)
{
    bool result = m_Objects.removeAll(obj) > 0 ? true : false;

    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
    return result;
}

void GroupObjects::del(const int &ind)
{
    m_Objects.remove(ind);
    qDebug() << "Objects in group" << m_Name << ":" << m_Objects.count();
}

QString GroupObjects::Name()
{
    return m_Name;
}

void GroupObjects::setName(const QString &n)
{
    m_Name = n;
}
