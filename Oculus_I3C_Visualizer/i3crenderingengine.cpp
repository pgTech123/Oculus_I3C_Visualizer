/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.cpp
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * TODO
 * *********************************************************/

#include "i3crenderingengine.h"

I3CRenderingEngine::I3CRenderingEngine(HDC hDC, HGLRC hRC)
{
    getOpenGLDevice(hDC, hRC);
}

I3CRenderingEngine::~I3CRenderingEngine()
{
}

void I3CRenderingEngine::openFile(std::string filename)
{

}

void I3CRenderingEngine::setFOV(float up, float down, float right, float left, int eye)
{

}

void I3CRenderingEngine::setPosition(float x, float y, float z)
{

}

void I3CRenderingEngine::setOrientation(float yaw, float pitch, float roll)
{

}

void I3CRenderingEngine::render(GLuint texId, int eye)
{
    glBindTexture(GL_TEXTURE_2D, texId);

    //Get Dimensions
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    float *arr = new float[w*h*4];
    for(int i = 0 ; i < w*h; i++){
        arr[i*4] = 0.5;
        arr[i*4+1] = 0.5;
        arr[i*4+2] = 0.5;
        arr[i*4+3] = 1.0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, arr);
    delete[] arr;
}

void I3CRenderingEngine::getOpenGLDevice(HDC hDC, HGLRC hRC)
{
    //WARNING: No error handeling in this function...
    cl_uint platAvail;
    cl_platform_id platform;

    //Get platform
    clGetPlatformIDs(1, &platform, &platAvail);

    cl_context_properties props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
                                     CL_GL_CONTEXT_KHR,   (cl_context_properties)hRC,
                                     CL_WGL_HDC_KHR,      (cl_context_properties)hDC, 0};

    clGetGLContextInfoKHR_fn _clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
    _clGetGLContextInfoKHR(props, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &m_device, NULL);

    //Verification DEBUG
    char GPU_name[100];
    clGetDeviceInfo(m_device, CL_DEVICE_NAME, sizeof(GPU_name), GPU_name, NULL);
    std::cout << GPU_name << std::endl;
}
