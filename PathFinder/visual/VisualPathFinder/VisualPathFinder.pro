#-------------------------------------------------
#
# Project created by QtCreator 2014-05-04T14:09:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualPathFinder
TEMPLATE = app

#LIBRARYPATH = /usr/local/Cellar
LIBRARYPATH = /users/moonraito/Desktop/A*/PathFinder/src
INCLUDEPATH += ../../src \
    $$LIBRARYPATH/boost/1.57.0/include

HEADERS += mainwindow.h
SOURCES += \
    main.cc \
    mainwindow.cc
FORMS    += mainwindow.ui

include(scene/scene.pri)

gcc:QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-local-typedefs
msvc:QMAKE_CXXFLAGS += /wd4100

CONFIG(release, debug|release): DEFINES += NDEBUG
