/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   OculusApp.cpp
 * Creation :   May 10th 2015
 * Purpose  :   Core of the App
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class (OcupusApp) is the core of the application.
 * It takes the Oculus inputs, makes calls to the rendering
 * engine, and display the result in the Oculus.
 * *********************************************************/

#include "oculusapp.h"

OculusApp::OculusApp():QGLWidget()
{
    ovr_Initialize(NULL);
    m_timer = new QTimer(this);

    m_RenderingEngine = NULL;
    m_bDismissHsw = true;
}

OculusApp::~OculusApp()
{
    this->stopRendering();
    this->shutdownOculusDevice();
    ovr_Shutdown();
}

int OculusApp::initOculusDevice()
{
    m_hmd = ovrHmd_Create(0);
    if(m_hmd){
        ovrHmd_ConfigureTracking(m_hmd, ovrTrackingCap_Orientation |
                                 ovrTrackingCap_MagYawCorrection |
                                 ovrTrackingCap_Position, 0);

        return OCULUS_NO_ERROR;
    }
    return OCULUS_NO_DEVICE_FOUND;
}

void OculusApp::shutdownOculusDevice()
{
    if(m_hmd){
        ovrHmd_Destroy(m_hmd);
        m_hmd = NULL;
    }
}

bool OculusApp::startRendering(std::string filename)
{
    if(!m_hmd){
        return false;
    }

    //Full screen on the Oculus in a Qt way :P
    this->move(QPoint(m_hmd->WindowsPos.x, m_hmd->WindowsPos.y));
    this->showFullScreen();

    //Configure Rendering
    this->initRenderingConfig();
    ovrHmd_ConfigureRendering(m_hmd, &m_renderingConfig.Config, m_hmd->DistortionCaps,
                              m_hmd->DefaultEyeFov, m_eyeRenderDesc );
    ovrHmd_AttachToWindow(m_hmd, m_renderingConfig.OGL.Window, NULL, NULL);

    //I3C Rendering Engine Initialisation
    m_RenderingEngine = new I3CRenderingEngine(wglGetCurrentDC(), wglGetCurrentContext());

    //Try to load the model on the engine. Return false and shutdown rendering engine if error.
    if(m_RenderingEngine->openFile(filename) != I3C_SUCCESS){
        this->stopRendering();
        return false;
    }

    //Setting Parameters
    this->createRenderingTexture(); //This function MUST be called before |this->initParameters()|
    this->initParameters();

    //Start the loop
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RenderScene()));
    m_timer->start();

    return true;
}

void OculusApp::stopRendering()
{
    disconnect(m_timer, SIGNAL(timeout()), this, SLOT(RenderScene()));

    if(m_RenderingEngine != NULL){
        delete m_RenderingEngine;
        m_RenderingEngine = NULL;
    }
}

void OculusApp::initRenderingConfig()
{
    m_renderingConfig.OGL.Header.API            = ovrRenderAPI_OpenGL;
    m_renderingConfig.OGL.Header.BackBufferSize = m_hmd->Resolution;
    m_renderingConfig.OGL.Header.Multisample    = 1;
    m_renderingConfig.OGL.Window                = (HWND)this->winId();
    m_renderingConfig.OGL.DC                    = wglGetCurrentDC();    //GetDC(m_renderingConfig.OGL.Window);
}

void OculusApp::initParameters()
{
    m_eyeFov[0] = m_hmd->DefaultEyeFov[ovrEye_Left];
    m_eyeFov[1] = m_hmd->DefaultEyeFov[ovrEye_Right];
    m_eyeRenderDesc[0] = ovrHmd_GetRenderDesc(m_hmd, ovrEye_Left, m_eyeFov[0]);
    m_eyeRenderDesc[1] = ovrHmd_GetRenderDesc(m_hmd, ovrEye_Right, m_eyeFov[1]);
    m_viewOffset[0] = m_eyeRenderDesc[0].HmdToEyeViewOffset;
    m_viewOffset[1] = m_eyeRenderDesc[1].HmdToEyeViewOffset;

    m_RenderingEngine->setFOV(m_eyeFov[0].DownTan, m_eyeFov[0].UpTan,
                             m_eyeFov[0].RightTan, m_eyeFov[0].LeftTan, LEFT_EYE);
    m_RenderingEngine->setFOV(m_eyeFov[1].DownTan, m_eyeFov[1].UpTan,
                             m_eyeFov[1].RightTan, m_eyeFov[1].LeftTan, RIGHT_EYE);
}

//This function must be called after initialisation of |m_RenderingEngine|
void OculusApp::createRenderingTexture()
{
    //For each eye
    for(int i = 0; i < 2; i++){
        //Get recommanded size by the Oculus
        ovrSizei recommenedTexSize = ovrHmd_GetFovTextureSize(m_hmd, m_hmd->EyeRenderOrder[i],
                                                               m_hmd->DefaultEyeFov[i],
                                                               1.0f);
        //Generate an OpenGL texture of this size
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, recommenedTexSize.w, recommenedTexSize.h,
                             0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        //Store texture
        m_eyeTexture[i].OGL.Header.API = ovrRenderAPI_OpenGL;
        m_eyeTexture[i].OGL.Header.TextureSize = recommenedTexSize;
        m_eyeTexture[i].OGL.Header.RenderViewport.Pos.x = 0;
        m_eyeTexture[i].OGL.Header.RenderViewport.Pos.y = 0;
        m_eyeTexture[i].OGL.Header.RenderViewport.Size.h = recommenedTexSize.h;
        m_eyeTexture[i].OGL.Header.RenderViewport.Size.w = recommenedTexSize.w;
        m_eyeTexture[i].OGL.TexId = texId;

        //Pass the texture to rederer
        m_RenderingEngine->setTexture(texId, i);
    }
}

void OculusApp::RenderScene()
{
    //Health & Safety warning
    ovrHmd_GetHSWDisplayState(m_hmd, &m_hswDisplayState);
    if(m_hswDisplayState.Displayed && m_bDismissHsw){
        ovrHmd_DismissHSWDisplay(m_hmd);
    }

    //Position Variables
    float yaw, pitch, roll;
    ovrPosef eyeRenderPose[2];

    //Verify that we have at least the orientation or position of the device
    ovrTrackingState ts = ovrHmd_GetTrackingState(m_hmd, ovr_GetTimeInSeconds());
    if(ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)){

        ovrHmd_BeginFrame(m_hmd, 0);

        //For each eye: render
        for(int eyeIndex = 0; eyeIndex < 2; eyeIndex++){
            //Get latest position
            ovrHmd_GetEyePoses(m_hmd, 0, m_viewOffset, eyeRenderPose, NULL);

            //Update  Orientation
            OVR::Quatf orientation = eyeRenderPose[eyeIndex].Orientation;
            orientation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &pitch, &roll);
            //std::cout << yaw << ", " << pitch << ", " << roll << std::endl;   //DEBUG
            m_RenderingEngine->setOrientation(yaw, pitch, roll);

            //Update Position
            float x = eyeRenderPose[eyeIndex].Position.x;
            float y = eyeRenderPose[eyeIndex].Position.y;
            float z = eyeRenderPose[eyeIndex].Position.z;
            //std::cout << x << ", " << y << ", " << z << std::endl;   //DEBUG
            m_RenderingEngine->setPosition(x, y, z);

            //Rendering
            m_RenderingEngine->render(eyeIndex);
        }

        //Prepearing to display
        ovrGLTexture eyeTex[2];
        for (int i = 0; i<2; i++)
        {
            eyeTex[i].OGL.Header.API = ovrRenderAPI_OpenGL;
            eyeTex[i].OGL.Header.TextureSize = m_eyeTexture[i].Texture.Header.TextureSize;
            eyeTex[i].OGL.Header.RenderViewport.Pos.x = 0;
            eyeTex[i].OGL.Header.RenderViewport.Pos.y = 0;
            eyeTex[i].OGL.Header.RenderViewport.Size.h = m_eyeTexture[i].OGL.Header.TextureSize.h;
            eyeTex[i].OGL.Header.RenderViewport.Size.w = m_eyeTexture[i].OGL.Header.TextureSize.w;
            eyeTex[i].OGL.TexId = m_eyeTexture[i].OGL.TexId;
        }

        ovrHmd_EndFrame(m_hmd, eyeRenderPose, &eyeTex[0].Texture);
    }
}
