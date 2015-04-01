#ifndef OCULUS_H
#define OCULUS_H

#include <Windows.h>
#include "OVR_CAPI.h"
#include "Kernel/OVR_Math.h"
#include "renderingwidget.h"
#include <string>
#include <iostream>

using namespace std;


//ERRORS DEFINITION
#define OCULUS_NO_ERROR            0
#define OCULUS_NO_DEVICE_FOUND     100

using namespace OVR;

DWORD WINAPI renderWorkFunction(LPVOID lpParameter);

bool positionTracked(ovrTrackingState ts);

void render(ovrHmd hmd,
            ovrVector3f eyeViewOffset[2],
            ovrTrackingState ts,
            RenderingWidget* renderingWidget);

typedef struct{
    bool end;
    ovrHmd *hmd;
    RenderingWidget* p_renderingWidget;
}ThreadData;


class Oculus
{
public:
    explicit Oculus();
    virtual ~Oculus();

    int initOculus();
    void render(string filename);
    void shutdownOculus();

private:
    DWORD m_threadID;
    HANDLE m_threadHandle;
    ThreadData data;

    ovrHmd m_hmd;

    RenderingWidget* m_RenderingWidget;
};

#endif // OCULUS_H
