#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T11:19:22
#
#-------------------------------------------------

QT       += core gui
QMAKE_LFLAGS += /INCREMENTAL:NO

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Oculus_I3C_Visualizer
TEMPLATE = app

INCLUDEPATH += "OculusSDK/LibOVR/Src"
INCLUDEPATH += "OculusSDK/LibOVR/Include"

SOURCES += main.cpp\
        mainwindow.cpp \
    oculus.cpp \
    renderingwidget.cpp \
    I3COculusEngine/i3coculusengine.cpp \
    I3COculusEngine/Utils/gvbinaryfunctions.cpp \
    I3COculusEngine/Utils/gvtransform.cpp \
    I3COculusEngine/i3ccube.cpp \
    OpenCL_Utils/cl_utils.cpp \
    I3COculusEngine/i3ccubecpu.cpp \
    I3COculusEngine/i3ccubegpu.cpp

HEADERS  += mainwindow.h \
    oculus.h \
    renderingwidget.h \
    I3COculusEngine/i3coculusengine.h \
    I3COculusEngine/Utils/gvbinaryfunctions.h \
    I3COculusEngine/Utils/gvtransform.h \
    I3COculusEngine/i3ccube.h \
    OpenCL_Utils/cl_utils.h \
    I3COculusEngine/i3ccubecpu.h \
    I3COculusEngine/i3ccubegpu.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010/ -llibovr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010/ -llibovrd

LIBS += -lWinmm
LIBS += -lws2_32

INCLUDEPATH += $$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010
DEPENDPATH += $$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010

win32: LIBS += -L$$PWD/OpenCL/lib/x86/ -lOpenCL

INCLUDEPATH += $$PWD/OpenCL/include
DEPENDPATH += $$PWD/OpenCL/include
