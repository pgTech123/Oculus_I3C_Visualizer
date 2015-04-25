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
        //cout <<"NO error" << endl;
        return OCULUS_NO_ERROR;
    }

    // DEBUG
    //cout << "No device found" << endl;
    return OCULUS_NO_DEVICE_FOUND;
}

void Oculus::render(string filename)
{
    //DO NOT ALLOW HAVE THIS THREAD RUNNING TWICE
    if(m_threadID == 0 && m_hmd)
    {
        //Creation of the rendering window
        m_RenderingWidget = new RenderingWidget();
        m_RenderingWidget->setFilename(filename);

        //Data that the thread needs to have a reference to
        data.hmd = &m_hmd;
        data.p_renderingWidget = m_RenderingWidget;
        data.end = false;

        m_threadHandle = CreateThread(0, 0, renderWorkFunction, &data, 0, &m_threadID);
    }
}

void Oculus::shutdownOculus()
{
    //The loop in the thread has a reference to |data.end| and will stop when true
    data.end = true;

    //We consider that a loop should not last more than 200ms.
    //This part of code could be improved to get better reliablity.
    Sleep(200);

    if(m_threadID != 0){
        CloseHandle(m_threadHandle);
        m_threadID = 0;
        delete m_RenderingWidget;
    }

    //Shutdown Oculus SDK
    ovrHmd_Destroy(m_hmd);
    ovr_Shutdown();
}

DWORD WINAPI renderWorkFunction(LPVOID lpParameter)
{
    //Get data from caller
    ThreadData *data = (ThreadData*)lpParameter;
    ovrHmd hmd = *(ovrHmd*)data->hmd;
    RenderingWidget *renderingWidget = (RenderingWidget*)data->p_renderingWidget;

    //Setup rendering widget
    renderingWidget->launchOculusEngine();
    ovrSizei resolution = hmd->Resolution;
    renderingWidget->setScreenResolution(resolution.w, resolution.h);   //MUST BE CALLED BEFORE SETTING FOV

    //Set the FOV for each eye:
    ovrFovPort eyeFov[2];
    eyeFov[0] = hmd->DefaultEyeFov[ovrEye_Left];
    eyeFov[1] = hmd->DefaultEyeFov[ovrEye_Right];
    renderingWidget->setFOVLeft(eyeFov[0].DownTan, eyeFov[0].UpTan, eyeFov[0].RightTan, eyeFov[0].LeftTan);
    renderingWidget->setFOVRight(eyeFov[1].DownTan, eyeFov[1].UpTan, eyeFov[1].RightTan, eyeFov[1].LeftTan);

    // Oculus usefull variable
    ovrFrameTiming frameTiming;
    ovrEyeRenderDesc EyeRenderDesc[2];
    EyeRenderDesc[0] = ovrHmd_GetRenderDesc(hmd, ovrEye_Left, eyeFov[0]);
    EyeRenderDesc[1] = ovrHmd_GetRenderDesc(hmd, ovrEye_Right, eyeFov[1]);
    ovrVector3f eyeViewOffset[2] = {EyeRenderDesc[0].HmdToEyeViewOffset,
                                    EyeRenderDesc[1].HmdToEyeViewOffset};

    // DEBUG
    //cout << "offset R : " << eyeViewOffset[0].x  << ", "<< eyeViewOffset[0].y  << ", "<<eyeViewOffset[0].z <<endl;
    //cout << "offset L : " << eyeViewOffset[1].x  << ", "<< eyeViewOffset[1].y  << ", "<<eyeViewOffset[1].z <<endl;

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

    renderingWidget->destroyOculusEngine();//Must be called within the thread
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
        //Get Head Position
        pose = ts.HeadPose.ThePose;
        pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

        eye = hmd->EyeRenderOrder[eyeIndex];
        headPose = ovrHmd_GetHmdPosePerEye(hmd, eye);

        //Update rendering widget
        renderingWidget->setRotation(yaw, pitch, roll);

        //Actual rendering
        if(eye == ovrEye_Left){
            renderingWidget->setLeftEyePosition(headPose.Position.x + eyeViewOffset[ovrEye_Left].x,
                                                headPose.Position.y + eyeViewOffset[ovrEye_Left].y,
                                                headPose.Position.z + eyeViewOffset[ovrEye_Left].z);
            renderingWidget->renderLeftEye();
        }
        else if(eye == ovrEye_Right){
            renderingWidget->setRightEyePosition(headPose.Position.x + eyeViewOffset[ovrEye_Right].x,
                                              headPose.Position.y + eyeViewOffset[ovrEye_Right].y,
                                              headPose.Position.z + eyeViewOffset[ovrEye_Right].z);
            renderingWidget->renderRightEye();
        }
    }
}
