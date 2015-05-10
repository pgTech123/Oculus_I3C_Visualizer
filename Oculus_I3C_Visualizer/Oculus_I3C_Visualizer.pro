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
    oculusapp.cpp

HEADERS  += mainwindow.h \
    oculusapp.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010/ -lLibovr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Debug/VS2010/ -lLibovr

LIBS += -lWinmm
LIBS += -lws2_32

INCLUDEPATH += $$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010
DEPENDPATH += $$PWD/OculusSDK/LibOVR/Lib/Windows/Win32/Release/VS2010

win32: LIBS += -L$$PWD/OpenCL/lib/x86/ -lOpenCL

INCLUDEPATH += $$PWD/OpenCL/include
DEPENDPATH += $$PWD/OpenCL/include
