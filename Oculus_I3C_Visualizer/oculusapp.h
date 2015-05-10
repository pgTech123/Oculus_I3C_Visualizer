#ifndef OCULUSAPP_H
#define OCULUSAPP_H

#include <QGLWidget>
#include <string>
#include <math.h>
#include <WinUser.h>
#include "OVR_CAPI_0_5_0.h"
#include "OVR_CAPI_GL.h"

//DEBUG
#include <iostream>
//ENDDEBUG

#define OCULUS_NO_ERROR            0
#define OCULUS_NO_DEVICE_FOUND     101


class OculusApp: public QGLWidget
{

public:
    OculusApp();
    ~OculusApp();

    int initOculusDevice();
    void shutdownOculusDevice();
    void startRendering(std::string filename);

private:
    void initRenderingConfig();
    void createRenderingTexture();

    void initializeGL();

private:
    ovrHmd m_hmd;

    ovrGLConfig m_renderingConfig;
    ovrEyeRenderDesc m_eyeRenderDesc[2];

    //Textures
    GLuint m_texId[2];  //No need... make this local
    ovrGLTexture m_eyeTexture[2];
};

#endif // OCULUSAPP_H
