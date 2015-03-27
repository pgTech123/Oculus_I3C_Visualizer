#include "oculus.h"

Oculus::Oculus()
{
    m_threadID = 0;
}

Oculus::~Oculus()
{
    //If not shutdown, shutdown
}

int Oculus::initOculus()
{
    ovr_Initialize();
    m_hmd = ovrHmd_Create(0);

    if(m_hmd){
        ovrHmd_ConfigureTracking(m_hmd, ovrTrackingCap_Orientation |
                                        ovrTrackingCap_MagYawCorrection |
                                        ovrTrackingCap_Position, 0);

        // DEBUG
        cout <<"NO error" << endl;
        return OCULUS_NO_ERROR;
    }

    // DEBUG
    cout << "No device found" << endl;
    return OCULUS_NO_DEVICE_FOUND;
}

void Oculus::render(const char* filename)
{
    //DO NOT ALLOW HAVE THIS THREAD RUNNING TWICE
    if(m_threadID == 0 && m_hmd)
    {
        //Creation of the rendering window
        m_RenderingWidget = new RenderingWidget();
        m_RenderingWidget->setFilename(filename);

        data.hmd = &m_hmd;
        data.p_renderingWidget = m_RenderingWidget;
        data.end = false;

        m_threadHandle = CreateThread(0, 0, renderWorkFunction, &data, 0, &m_threadID);
    }
}

void Oculus::shutdownOculus()
{
    data.end = true;
    Sleep(200);
    if(m_threadID != 0){
        CloseHandle(m_threadHandle);
        m_threadID = 0;
        delete m_RenderingWidget;
    }

    ovrHmd_Destroy(m_hmd);
    ovr_Shutdown();
}

DWORD WINAPI renderWorkFunction(LPVOID lpParameter)
{
    //Get data from caller
    ThreadData *data = (ThreadData*)lpParameter;
    ovrHmd hmd = *(ovrHmd*)data->hmd;
    RenderingWidget *renderingWidget = (RenderingWidget*)data->p_renderingWidget;
    renderingWidget->launchOculusEngine();
    ovrSizei resolution = hmd->Resolution;
    renderingWidget->setScreenResolution(resolution.w, resolution.h);

    //Set the FOV for each eye:
    ovrFovPort FOV = hmd->DefaultEyeFov[ovrEye_Left];
    renderingWidget->setFOVLeft(FOV.DownTan, FOV.UpTan, FOV.RightTan, FOV.LeftTan);
    FOV = hmd->DefaultEyeFov[ovrEye_Right];
    renderingWidget->setFOVRight(FOV.DownTan, FOV.UpTan, FOV.RightTan, FOV.LeftTan);

    // Oculus usefull variable
    ovrFrameTiming frameTiming;
    ovrEyeRenderDesc EyeRenderDesc[2];
    ovrVector3f eyeViewOffset[2] = {EyeRenderDesc[0].HmdToEyeViewOffset,
                                    EyeRenderDesc[1].HmdToEyeViewOffset};

    //Rendering Loop
    while(hmd && !data->end)
    {
        frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);
        ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, frameTiming.ScanoutMidpointSeconds);

        if(positionTracked(ts)){
            render(hmd, eyeViewOffset, ts, renderingWidget);
            Sleep(10);
            ovrHmd_EndFrameTiming(hmd);
        }
    }


    renderingWidget->destroyOculusEngine();
    // DEBUG
    cout << "out" << endl;

    return 0;
}

bool positionTracked(ovrTrackingState ts)
{
    return ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked);
}

void render(ovrHmd hmd, ovrVector3f eyeViewOffset[2],
            ovrTrackingState ts, RenderingWidget* renderingWidget)
{
    float yaw, pitch, roll;
    ovrPosef headPose;
    Posef pose;
    ovrEyeType eye;

    for(int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
    {
        pose = ts.HeadPose.ThePose;
        pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

        eye = hmd->EyeRenderOrder[eyeIndex];

        headPose = ovrHmd_GetHmdPosePerEye(hmd, eye);

        //Update rendering widget
        renderingWidget->setRotation(yaw, pitch, roll);
        if(eye == ovrEye_Left){
            renderingWidget->setLeftEyePosition(headPose.Position.x + eyeViewOffset[eye].x,
                                             headPose.Position.y + eyeViewOffset[eye].y,
                                             headPose.Position.z + eyeViewOffset[eye].z);
            renderingWidget->renderLeftEye();
        }
        else if(eye == ovrEye_Right){
            renderingWidget->setRightEyePosition(headPose.Position.x + eyeViewOffset[eye].x,
                                              headPose.Position.y + eyeViewOffset[eye].y,
                                              headPose.Position.z + eyeViewOffset[eye].z);
            renderingWidget->renderRightEye();
        }
    }
}
