/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.h
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class creates OpenCL kernels and makes the
 * synchronisation between OpenGL and OpenCL texture
 * ownership. It also reads the I3C file and creates
 * |I3CCubes| classes accordingly to what's written in
 * the file. OCL Kernels are passed to these childs so
 * each cube can access and run code on the GPU. This
 * is like the Mothership.
 * *********************************************************/

#ifndef I3CRENDERINGENGINE_H
#define I3CRENDERINGENGINE_H

#include <string>
#include <Windows.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <GL/glut.h>
#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "utils/gvtransform.h"
#include "utils/gvbinaryfunctions.h"

//DEBUG
#include <iostream>
//ENDDEBUG

#define LEFT_EYE    0
#define RIGHT_EYE   1

#define FOCAL_LENGHT    300

/* Error Definition */
#define I3C_SUCCESS             0
#define UNABLE_TO_OPEN_FILE     101
#define INVALID_CUBE_SIZE       102
#define SIZE_NOT_BASE_2         103
#define FILE_CORRUPTED          104

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
    int openFile(std::string filename);
    void setFOV(float up, float down, float right, float left, int eye = 0);
    void setTexture(GLuint texId, int eye);

    //These functions are typically called in every loop
    void setPosition(float x, float y, float z);
    void setOrientation(float yaw, float pitch, float roll);
    void render(int eye = 0);

private:
    //This function is meant to be called only once at the begining (i.e. in the constructor)
    void getOpenGLDevice(HDC hDC, HGLRC hRC);

    void createKernels();
    void allocateMemory();

    // If Sources_OCL.success is false, an error was encountered and the structure
    // may not be properly set.
    // By default: CL sources max 1 Mo
    Sources_OCL loadCLSource(char* filename, unsigned int max_length=100000);

    //I3C File reading
    int readImageFile(std::fstream *file);
    int verifyAndAssignSideLength(int iSideLength);
    void setNumberOfLevels();
    void readNumOfMaps(std::fstream *file);
    int readCubes(std::fstream *file);
    int readPixelCubes(std::fstream *file);
    int readIndexCubes(std::fstream *file);
    int readMap(fstream *file, unsigned char* ucMap, int* iNumOfPix);
    void clearCubesInMemory();

private:
    //Texture Size
    int m_iWidth[2];  //Left/Right
    int m_iHeight[2]; //Left/Right
    int FOV[2][4];

    //OpenCL General
    cl_device_id m_device;
    cl_context m_context;
    cl_command_queue m_queue;

    //Memory Global
    cl_mem m_clTexture[2];  //Left/Right
    cl_mem m_clFOV[2];      //Left/Right
    cl_mem m_clReferenceCubeMap;
    cl_mem m_clPixel;
    cl_mem m_clNumOfLevel;

    cl_mem m_clRotatedCorners;
    cl_mem m_clBoundingRect;
    cl_mem m_clChildId_memStatusBit;

    //Code that runs on GPU
    cl_program m_program;
    cl_kernel m_kernelClearCornersComputed;
    cl_kernel m_kernelRender[2];  //Left/Right

    //--------  IMAGE  ----------
    Transform m_transform;

    //Image Property
    int m_iSideLenght;
    int m_iNumberOfLevels;
    int *m_iArrCubeAtLevel;
    int m_iTotalNumberOfCubes;
};

#endif // I3CRENDERINGENGINE_H
