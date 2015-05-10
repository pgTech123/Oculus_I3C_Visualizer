#include "oculusapp.h"

OculusApp::OculusApp():QGLWidget()
{
    ovr_Initialize(NULL);
}

OculusApp::~OculusApp()
{
    this->shutdownOculusDevice();
    ovr_Shutdown();
}

int OculusApp::initOculusDevice()
{
    m_hmd = ovrHmd_Create(0);
    if(m_hmd)
    {
        //Use Orientation, Position and MagnYawCorrection if available
        ovrHmd_ConfigureTracking(m_hmd, ovrTrackingCap_Orientation |
                                 ovrTrackingCap_MagYawCorrection |
                                 ovrTrackingCap_Position, 0);

        return OCULUS_NO_ERROR;
    }
    return OCULUS_NO_DEVICE_FOUND;
}

void OculusApp::shutdownOculusDevice()
{
    if(m_hmd)
    {
        ovrHmd_Destroy(m_hmd);
        m_hmd = NULL;
    }
}

void OculusApp::startRendering(std::string filename)
{
    if(!m_hmd){
        return;
    }

    //Show Full screen on the Oculus
    this->move(QPoint(m_hmd->WindowsPos.x, m_hmd->WindowsPos.y));
    this->showFullScreen();

    //Configure Rendering
    this->initRenderingConfig();
    ovrHmd_ConfigureRendering(m_hmd, &m_renderingConfig.Config, ovrDistortionCap_Vignette |
                              ovrDistortionCap_TimeWarp | ovrDistortionCap_Overdrive,
                              m_hmd->DefaultEyeFov, m_eyeRenderDesc );
    ovrHmd_AttachToWindow(m_hmd, m_renderingConfig.OGL.Window, NULL, NULL);

    //Textures
    this->createRenderingTexture();

    ovrHmd_DismissHSWDisplay(m_hmd);
    ovrHmd_BeginFrame(m_hmd, 0);
    ovrPosef EyeRenderPose[2];
    ovrHmd_EndFrame(m_hmd, EyeRenderPose, &m_eyeTexture[0].Texture);

    paintGL();

    //TODO: Display Health & Safety warning
    //TODO: Launch mainLoop
}

void OculusApp::initRenderingConfig()
{
    m_renderingConfig.OGL.Header.API            = ovrRenderAPI_OpenGL;
    m_renderingConfig.OGL.Header.BackBufferSize = m_hmd->Resolution;
    m_renderingConfig.OGL.Header.Multisample    = 0;
    m_renderingConfig.OGL.Window                = (HWND)this->winId();
    m_renderingConfig.OGL.DC                    = GetDC(m_renderingConfig.OGL.Window);
}

void OculusApp::createRenderingTexture()
{
    //For each eye
    for(int i = 0; i < 2; i++){
        //Get recommanded size by the Oculus
        ovrSizei recommenedTexSize = ovrHmd_GetFovTextureSize(m_hmd, m_hmd->EyeRenderOrder[i],
                                                               m_hmd->DefaultEyeFov[i],
                                                               1.0f);

        //Generate an OpenGL texture of this size
        glGenTextures(1, &m_texId[i]);
        glBindTexture(GL_TEXTURE_2D, m_texId[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, recommenedTexSize.w, recommenedTexSize.h,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);


        m_eyeTexture[i].OGL.Header.API = ovrRenderAPI_OpenGL;
        m_eyeTexture[i].OGL.Header.TextureSize = recommenedTexSize;
        m_eyeTexture[i].OGL.TexId = m_texId[i];
    }
}

void OculusApp::initializeGL()
{
    glClearColor(0.0, 1.0, 0.0, 0.0);
}
