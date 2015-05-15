#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T11:19:22
#
#-------------------------------------------------

QT       += core gui
QT += opengl
QMAKE_LFLAGS += /INCREMENTAL:NO

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Oculus_I3C_Visualizer
TEMPLATE = app

INCLUDEPATH += "OculusSDK/LibOVR/Src"
INCLUDEPATH += "OculusSDK/LibOVR/Include"

SOURCES += main.cpp\
        mainwindow.cpp \
    oculusapp.cpp \
    i3crenderingengine.cpp \
    utils/gvbinaryfunctions.cpp \
    utils/gvtransform.cpp \
    image_cubes/i3ccube.cpp \
    image_cubes/i3cpixelcube.cpp \
    image_cubes/i3creferencecube.cpp

HEADERS  += mainwindow.h \
    oculusapp.h \
    i3crenderingengine.h \
    utils/gvbinaryfunctions.h \
    utils/gvtransform.h \
    image_cubes/i3ccube.h \
    image_cubes/i3cpixelcube.h \
    image_cubes/i3creferencecube.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010/ -lLibovr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Debug/VS2010/ -lLibovr

LIBS += -lWinmm
LIBS += -lws2_32

INCLUDEPATH += $$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010
DEPENDPATH += $$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010

win32: LIBS += -L$$PWD/OpenCL/lib/x86/ -lOpenCL
LIBS += -L$$PWD/OpenCL/lib/x86/ -lglut32
LIBS += -L$$PWD/OpenCL/lib/x86/ -lglew32
LIBS += -L$$PWD/OpenCL/lib/x86/ -lGLAux

INCLUDEPATH += $$PWD/OpenCL/include
DEPENDPATH += $$PWD/OpenCL/include
