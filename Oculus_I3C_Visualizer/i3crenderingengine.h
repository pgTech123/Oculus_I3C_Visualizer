/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.h
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * TODO
 * *********************************************************/

#ifndef I3CRENDERINGENGINE_H
#define I3CRENDERINGENGINE_H

#include <string>
#include <Windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glut.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>

//DEBUG
#include <iostream>
//ENDDEBUG

#define LEFT_EYE    0
#define RIGHT_EYE   1

typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)(const cl_context_properties * /* properties */,
                                                                    cl_gl_context_info /* param_name */,
                                                                    size_t /* param_value_size */,
                                                                    void * /* param_value */,
                                                                    size_t * /*param_value_size_ret*/);

typedef struct{
    char *sources;
    size_t source_size;
    bool success;
}Sources_OCL;


class I3CRenderingEngine
{
public:
    I3CRenderingEngine(HDC hDC, HGLRC hRC);
    ~I3CRenderingEngine();

    //These Functions are typically called once (or once for each eye)
    void openFile(std::string filename);
    void setFOV(float up, float down, float right, float left, int eye = 0);
    void setTexture(GLuint texId, int eye);

    //These functions are typically called in every loop
    void setPosition(float x, float y, float z);
    void setOrientation(float yaw, float pitch, float roll);
    void render(GLuint texId, int eye = 0); //TODO: remove textId

private:
    void getOpenGLDevice(HDC hDC, HGLRC hRC);
    void createKernels();

    // If Sources_OCL.success is false, an error was encountered and the structure
    // may not be properly set.
    // By default: CL sources max 1 Mo
    Sources_OCL loadCLSource(char* filename, unsigned int max_length=100000);

private:
    //
    cl_device_id m_device;
    cl_context m_context;
    cl_command_queue m_queue;

    //Memory
    cl_mem m_clTexture[2];
    cl_mem m_clI3CImage;
    cl_mem m_clCubeStack;   //TODO: to use

    //Software that runs on GPU
    cl_program m_program;
    cl_kernel m_kernel;
};

#endif // I3CRENDERINGENGINE_H
