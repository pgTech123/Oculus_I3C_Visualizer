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
    m_clTexture[0] = NULL;
    m_clTexture[1] = NULL;
    m_clI3CImage = NULL;
    m_kernel[0] = NULL;
    m_kernel[1] = NULL;
    m_program = NULL;

    getOpenGLDevice(hDC, hRC);
}

I3CRenderingEngine::~I3CRenderingEngine()
{
    if(m_kernel[0] != NULL){
        clReleaseKernel(m_kernel[0]);
    }
    if(m_kernel[1] != NULL){
        clReleaseKernel(m_kernel[1]);
    }
    if(m_program != NULL){
        clReleaseProgram(m_program);
    }
    if(m_clTexture[0] != NULL){
        clReleaseMemObject(m_clTexture[0]);
    }
    if(m_clTexture[1] != NULL){
        clReleaseMemObject(m_clTexture[1]);
    }
    if(m_clI3CImage != NULL){
        clReleaseMemObject(m_clI3CImage);
    }
    clReleaseCommandQueue(m_queue);
    clReleaseContext(m_context);
    clReleaseDevice(m_device);
}

void I3CRenderingEngine::openFile(std::string filename)
{
    //TODO: read file
    //Create CL Buffer Object
}

void I3CRenderingEngine::setFOV(float up, float down, float right, float left, int eye)
{

}

void I3CRenderingEngine::setTexture(GLuint texId, int eye)
{
    if(eye > 1){    //Eye out of range...
        return;
    }

    //Reference OpenGL memory to OpenCL
    glBindTexture(GL_TEXTURE_2D, texId);
    m_clTexture[eye] = clCreateFromGLTexture(m_context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, texId, NULL);

    //Set Texture Argument
    clSetKernelArg(m_kernel[eye], 0, sizeof(m_clTexture[eye]), &m_clTexture[eye]);

    //Get data
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_iWidth[eye]);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_iHeight[eye]);

    /*glBindTexture(GL_TEXTURE_2D, texId);

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
    delete[] arr;*/
}

void I3CRenderingEngine::setPosition(float x, float y, float z)
{

}

void I3CRenderingEngine::setOrientation(float yaw, float pitch, float roll)
{

}

void I3CRenderingEngine::render(int eye)
{
    //Give texture ownership to OpenCL
    glFinish();
    cl_int error = clEnqueueAcquireGLObjects(m_queue, 1,  &m_clTexture[eye], 0, 0, NULL);
    if(error != CL_SUCCESS){
        std::cout << "Aquirering error..." << std::endl;
    }

    //Render!
    size_t a[2] = {m_iWidth[eye], m_iHeight[eye]};
    error = clEnqueueNDRangeKernel(m_queue, m_kernel[eye], 2, NULL, a , NULL, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        std::cout << "Task error..." << std::endl;
    }

    //Give back texture ownership to OpenGL
    clFinish(m_queue);
    error = clEnqueueReleaseGLObjects(m_queue, 1,  &m_clTexture[eye], 0, 0, NULL);
    if(error == CL_OUT_OF_RESOURCES ){
        std::cout << "Releasing error..." << std::endl;
    }
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

    //Find device on which OpenGL runs
    clGetGLContextInfoKHR_fn _clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
    _clGetGLContextInfoKHR(props, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &m_device, NULL);

    //Create Context and command queue on that device
    m_context = clCreateContext(props, 1, &m_device, 0, 0, NULL);
    m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, NULL);
    this->createKernels();
}

void I3CRenderingEngine::createKernels()
{
    Sources_OCL sources = this->loadCLSource("./cl_program.cl");

    if(sources.success){
        std::cout << "CL Sources found" << std::endl;   //DEBUG

        //Create program
        m_program = clCreateProgramWithSource(m_context,  1, (const char **)&sources.sources,
                                                       (const size_t *)&sources.source_size, NULL);//*/
        //Compile Program
        cl_int error;
        error = clBuildProgram(m_program, 1, &m_device, NULL, NULL, NULL);
        if(error != CL_SUCCESS){
            std::cout << "Compilation error..." << std::endl;
        }

        //Create Kernels (left/right)
        m_kernel[0] = clCreateKernel(m_program, "clearImage", NULL);
        m_kernel[1] = clCreateKernel(m_program, "clearImage", NULL);
    }
    else{
        std::cout << "ERROR: CL Sources NOT found" << std::endl;   //DEBUG
    }
}

Sources_OCL I3CRenderingEngine::loadCLSource(char* filename, unsigned int max_length)
{
    Sources_OCL sources;

    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) {
        sources.success = false;
        return sources;
    }

    sources.sources = (char*)malloc(max_length);
    sources.source_size = fread(sources.sources, 1, max_length, fp);
    fclose(fp);

    sources.success = true;
    return sources;
}
