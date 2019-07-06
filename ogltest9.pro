#//////////////////////////////////////////
# тестовая работа к занятию https://youtu.be/TadCATIDZ2c
#//////////////////////////////////////////

QT       += core gui widgets

TARGET = ogltest9
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

SOURCES += \
        engineobject3d.cpp \
        eye.cpp \
        groupobjects.cpp \
        main.cpp \
        material.cpp \
        materiallibrary.cpp \
        object3d.cpp \
        oglwidget.cpp \
        skybox.cpp \
        transformational.cpp

HEADERS += \
        engineobject3d.h \
        eye.h \
        groupobjects.h \
        material.h \
        materiallibrary.h \
        object3d.h \
        oglwidget.h \
        skybox.h \
        transformational.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    models.qrc \
    shaders.qrc \
    textures.qrc

win32-g* {
LIBS += -lopengl32
}

win32-msvc* {
LIBS += lopengl32.lib
}
