#ifndef OCULUS_H
#define OCULUS_H

#include <Windows.h>
#include "OVR_CAPI.h"
#include "Kernel/OVR_Math.h"
#include "renderingwidget.h"

// DEBUG PURPOSE ONLY
#include <iostream>
using namespace std;
// END DEBUG PURPOSE

//ERRORS DEFINITION
#define OCULUS_NO_ERROR            0
#define OCULUS_NO_DEVICE_FOUND     100

using namespace OVR;

DWORD WINAPI renderWorkFunction(LPVOID lpParameter);

typedef struct{
    bool end;
    ovrHmd *hmd;
    const char* filename;
}ThreadData;


class Oculus
{
public:
    explicit Oculus();
    virtual ~Oculus();

    int initOculus();
    void render(const char* filename);
    void shutdownOculus();

private:
    DWORD m_threadID;
    HANDLE m_threadHandle;
    ThreadData data;

    ovrHmd m_hmd;
};

#endif // OCULUS_H
