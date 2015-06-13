QT       += core gui widgets
# For QGLWidget::convertToGLFormat
#QT += opengl

TARGET = earth10
TEMPLATE = app

SOURCES += main.cpp \
    trackball.cpp \
    gshhsdata.cpp \
    skybox.cpp

SOURCES += \
    mainwidget.cpp \
    geometryengine.cpp

HEADERS += \
    mainwidget.h \
    geometryengine.h \
    trackball.h \
    gshhsdata.h \
    gshhs.h \
    skybox.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

CONFIG += c++11

INCLUDEPATH += ../../SOIL/src
LIBS += -L. -lSOIL

DISTFILES += \
    EarthNormal.png \
    miramar_bk.tga \
    miramar_dn.tga \
    miramar_ft.tga \
    miramar_lf.tga \
    miramar_rt.tga \
    miramar_up.tga \
    miramar_up1.tga \
    earthalpha.png
