#-------------------------------------------------
#
# Project created by QtCreator 2015-04-13T20:41:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SceneMeshViewer
TEMPLATE = app
LIBS += -lcapnp -lcapnp-rpc -lkj -lkj-async

QMAKE_CXXFLAGS += -std=c++14
QMAKE_EXT_CPP += .c++

SOURCES += main.cpp\
        scene.capnp.c++ \
        MainWindow.cpp \
    MyGLWidget.cpp

HEADERS  += MainWindow.h \
        scene.capnp.h \
    MyGLWidget.h

FORMS    += MainWindow.ui
