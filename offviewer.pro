#-------------------------------------------------
#
# Project created by QtCreator 2012-03-16T18:51:34
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = offviewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    glwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    fshader.glsl \
    vshader.glsl \

RESOURCES += \
    resources.qrc
