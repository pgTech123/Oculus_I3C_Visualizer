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

class I3CRenderingEngine
{
public:
    I3CRenderingEngine(HDC hDC, HGLRC hRC);
    ~I3CRenderingEngine();

    void openFile(std::string filename);

    void setFOV(float up, float down, float right, float left, int eye = 0);

    void setPosition(float x, float y, float z);
    void setOrientation(float yaw, float pitch, float roll);

    void render(GLuint texId, int eye = 0);

private:
    void getOpenGLDevice(HDC hDC, HGLRC hRC);

private:
    cl_device_id m_device;
};

#endif // I3CRENDERINGENGINE_H
