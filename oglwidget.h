#ifndef OGLWIDGET_H
#define OGLWIDGET_H

#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QBasicTimer>

class EngineObject3D;
class Transformational;
class GroupObjects;
class Eye;
class SkyBox;

class OGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    OGLWidget(QWidget *parent = nullptr);
    ~OGLWidget();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void timerEvent(QTimerEvent* event);

    void initShaders();
    void initCube(float width, const QString &imgpath);

private:
    QMatrix4x4 m_PojectionMatrix;
    QOpenGLShaderProgram m_Program;
    QOpenGLShaderProgram m_ProgramSkyBox;
    QVector2D m_MousePosition;
    QQuaternion m_Rotation;
    QVector<EngineObject3D*> m_Objects;
    QVector<GroupObjects*> m_Groups;
    GroupObjects* m_GlobalGroup;
    Eye* m_Eye;
    SkyBox* m_SkyBox;
    QBasicTimer m_Timer;

    double m_AngleObject;
    double m_AngleGrop1;
    double m_AngleGrop2;
    double m_AngleGrop3;
    double m_AngleGrop4;
    double m_AngleGropMain;
    int currentGroup = 0;
};

#endif // OGLWIDGET_H
