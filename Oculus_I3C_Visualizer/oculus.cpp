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
    //DO NOT ALLOW TO START THIS THREAD TWICE
    if(m_threadID == 0 && m_hmd)
    {
        data.hmd = &m_hmd;
        data.filename = filename;

        m_threadHandle = CreateThread(0, 0, renderWorkFunction, &data, 0, &m_threadID);
    }
}

void Oculus::shutdownOculus()
{
    if(m_threadID != 0){
        CloseHandle(m_threadHandle);
        m_threadID = 0;
    }
    ovrHmd_Destroy(m_hmd);
    ovr_Shutdown();
}

DWORD WINAPI renderWorkFunction(LPVOID lpParameter)
{
    ThreadData *data = (ThreadData*)lpParameter;
    ovrHmd hmd = *(ovrHmd*)data->hmd;
    const char* filename = data->filename;

    ovrFrameTiming frameTiming;

    // Rendering settings
    Sizei recommenedTex0Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left,
                                                    hmd->DefaultEyeFov[0], 1.0f);
    Sizei recommenedTex1Size = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right,
                                                    hmd->DefaultEyeFov[1], 1.0f);


    while(hmd){
        frameTiming = ovrHmd_BeginFrameTiming(hmd, 0);
        ovrTrackingState TrackingState = ovrHmd_GetTrackingState(hmd,
                                                          frameTiming.ScanoutMidpointSeconds);

        if(TrackingState.StatusFlags & (ovrStatus_OrientationTracked |
                                          ovrStatus_PositionTracked)){
            Posef pose = TrackingState.HeadPose.ThePose;
            cout << "Pos and ori tracked!" << endl;
            float yaw, pitch, roll;
            pose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);

            cout << "yaw: "   << RadToDegree(yaw)   << endl;
            cout << "pitch: " << RadToDegree(pitch) << endl;
            cout << "roll: "  << RadToDegree(roll)  << endl;

            cout << "Filename: " << filename << endl;
            Sleep(50);

            ovrHmd_EndFrameTiming(hmd);
        }
    }

    return 0;
}
