/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   OculusApp.h
 * Creation :   May 10th 2015
 * Purpose  :   Core of the App
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class (OcupusApp) is the core of the application.
 * It takes the Oculus inputs, makes calls to the rendering
 * engine, and display the result in the Oculus.
 * *********************************************************/

#ifndef OCULUSAPP_H
#define OCULUSAPP_H

#include <QGLWidget>
#include <QTimer>
#include <string>
#include <math.h>
#include <WinUser.h>
#include "OVR_CAPI_0_5_0.h"
#include "Extras/OVR_Math.h"
#include "OVR_CAPI_GL.h"
#include "i3crenderingengine.h"

//DEBUG
#include <iostream>
//ENDDEBUG

#define OCULUS_NO_ERROR            0
#define OCULUS_NO_DEVICE_FOUND     101

//TODO: when a key pressed -> m_bDismissHsw = true

class OculusApp: public QGLWidget
{
    Q_OBJECT

public:
    OculusApp();
    ~OculusApp();

    int initOculusDevice();
    void shutdownOculusDevice();
    void startRendering(std::string filename);

private:
    void initRenderingConfig();
    void initParameters();
    void createRenderingTexture();

private slots:
    void RenderScene();

private:
    ovrHmd m_hmd;

    //Loop Timer
    QTimer *m_timer;

    //General
    ovrGLConfig m_renderingConfig;
    ovrEyeRenderDesc m_eyeRenderDesc[2];
    ovrEyeType m_eye;
    ovrFovPort m_eyeFov[2];
    ovrVector3f m_viewOffset[2];

    //Health and Safety Warnings
    bool m_bDismissHsw;
    ovrHSWDisplayState m_hswDisplayState;

    //Textures
    ovrGLTexture m_eyeTexture[2];

    //Rendering Engine
    I3CRenderingEngine m_RenderingEngine;
};

#endif // OCULUSAPP_H
