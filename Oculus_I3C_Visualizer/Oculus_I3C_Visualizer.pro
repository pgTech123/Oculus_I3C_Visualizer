#-------------------------------------------------
#
# Project created by QtCreator 2015-03-08T11:19:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Oculus_I3C_Visualizer
TEMPLATE = app

INCLUDEPATH += "OculusSDK/LibOVR/Src"
INCLUDEPATH += "OculusSDK/LibOVR/Include"

SOURCES += main.cpp\
        mainwindow.cpp \
    oculus.cpp

HEADERS  += mainwindow.h \
    oculus.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010/ -llibovr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010/ -llibovrd

LIBS += -lWinmm
LIBS += -lws2_32

INCLUDEPATH += $$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010
DEPENDPATH += $$PWD/OculusSDK/LibOVR/Lib/Win32/VS2010
