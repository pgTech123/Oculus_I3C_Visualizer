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
        data.hmd = &m_hmd;
        data.filename = filename;
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
    }

    ovrHmd_Destroy(m_hmd);
    ovr_Shutdown();
}

DWORD WINAPI renderWorkFunction(LPVOID lpParameter)
{
    //Data from the Oculus class
    ThreadData *data = (ThreadData*)lpParameter;
    ovrHmd hmd = *(ovrHmd*)data->hmd;
    const char* filename = data->filename;

    // Oculus timing for prediction
    ovrFrameTiming frameTiming;

    // Initialize Rendering
    Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,
                                                    hmd->DefaultEyeFov[0], 1.0f);
    Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right,
                                                    hmd->DefaultEyeFov[1], 1.0f);

    ovrEyeRenderDesc EyeRenderDesc[2];
    ovrVector3f eyeViewOffset[2] = {EyeRenderDesc[0].HmdToEyeViewOffset,
                                    EyeRenderDesc[1].HmdToEyeViewOffset};



    //Creation of the rendering window
    RenderingWidget *renderWidget = new RenderingWidget();
    renderWidget->openFile(filename);

    while(hmd && !data->end){
        frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);
        ovrTrackingState TrackingState = ovrHmd_GetTrackingState(hmd,
                                              frameTiming.ScanoutMidpointSeconds);

        if(TrackingState.StatusFlags & (ovrStatus_OrientationTracked |
                                          ovrStatus_PositionTracked))
        {
            float yaw, pitch, roll;
            ovrPosef headPose;

            for(int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
            {
                Posef pose = TrackingState.HeadPose.ThePose;
                pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

                ovrEyeType eye = hmd->EyeRenderOrder[eyeIndex];

                headPose = ovrHmd_GetHmdPosePerEye(hmd, eye);

                //Update rendering widget
                renderWidget->setRotation(yaw, pitch, roll);
                if(eye == ovrEye_Left){
                    renderWidget->setLeftEyePosition(headPose.Position.x + eyeViewOffset[eye].x,
                                                     headPose.Position.y + eyeViewOffset[eye].y,
                                                     headPose.Position.z + eyeViewOffset[eye].z);
                    renderWidget->renderLeftEye();
                }
                else if(eye == ovrEye_Right){
                    renderWidget->setRightEyePosition(headPose.Position.x + eyeViewOffset[eye].x,
                                                      headPose.Position.y + eyeViewOffset[eye].y,
                                                      headPose.Position.z + eyeViewOffset[eye].z);
                    renderWidget->renderRightEye();
                }
            }

            Sleep(10);
            ovrHmd_EndFrameTiming(hmd);
        }
    }

    // Delete widget
    delete renderWidget;

    // DEBUG
    cout << "out" << endl;
    return 0;
}
