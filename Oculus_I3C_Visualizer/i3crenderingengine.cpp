/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.cpp
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

#include "i3crenderingengine.h"

I3CRenderingEngine::I3CRenderingEngine(HDC hDC, HGLRC hRC)
{
    //OpenCL init
    m_clTexture[0] = NULL;
    m_clTexture[1] = NULL;
    m_clFOV[0] = NULL;
    m_clFOV[1] = NULL;
    m_clCubeDstSorted = NULL;
    m_kernelClearImage[0] = NULL;
    m_kernelClearImage[1] = NULL;
    m_kernelComputeChildCorners = NULL;
    m_program = NULL;

    getOpenGLDevice(hDC, hRC);

    //Image init
    m_iArrCubeAtLevel = NULL;
    m_Cubes = NULL;
}

I3CRenderingEngine::~I3CRenderingEngine()
{
    if(m_kernelClearImage[0] != NULL){
        clReleaseKernel(m_kernelClearImage[0]);
    }
    if(m_kernelClearImage[1] != NULL){
        clReleaseKernel(m_kernelClearImage[1]);
    }
    if(m_kernelComputeChildCorners != NULL){
        clReleaseKernel(m_kernelComputeChildCorners);
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
    if(m_clFOV[0] != NULL){
        clReleaseMemObject(m_clFOV[0]);
    }
    if(m_clFOV[1] != NULL){
        clReleaseMemObject(m_clFOV[1]);
    }
    if(m_clCubeDstSorted != NULL){
        clReleaseMemObject(m_clCubeDstSorted);
    }
    clReleaseCommandQueue(m_queue);
    clReleaseContext(m_context);
    clReleaseDevice(m_device);

    //Release Image Data
    clearCubesInMemory();
}

int I3CRenderingEngine::openFile(std::string filename)
{
    std::fstream file;

    file.open(filename);

    if(file.is_open()){
        int iError = readImageFile(&file);
        file.close();
        return iError;
    }
    else{
        return UNABLE_TO_OPEN_FILE;
    }
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
    clSetKernelArg(m_kernelClearImage[eye], 0, sizeof(m_clTexture[eye]), &m_clTexture[eye]);

    //Get data
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_iWidth[eye]);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_iHeight[eye]);
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

    //Clear Texture
    size_t a[2] = {m_iWidth[eye], m_iHeight[eye]};
    error = clEnqueueNDRangeKernel(m_queue, m_kernelClearImage[eye], 2, NULL, a , NULL, 0, NULL, NULL);
    if(error != CL_SUCCESS){
        std::cout << "Task error..." << std::endl;
    }
    //Render!
    //TODO: call render on last cube in the array
    //m_ucCubeDstSorted and cl equivalent

    //Give back texture ownership to OpenGL
    clFinish(m_queue);
    error = clEnqueueReleaseGLObjects(m_queue, 1,  &m_clTexture[eye], 0, 0, NULL);
    if(error != CL_SUCCESS){
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

        //Create Kernels
        m_kernelClearImage[0] = clCreateKernel(m_program, "clearImage", NULL);  //Left Texture
        m_kernelClearImage[1] = clCreateKernel(m_program, "clearImage", NULL);  //Right Texture
        m_kernelComputeChildCorners = clCreateKernel(m_program, "computeSubCorners", NULL);
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

//-----------------------    READING IMAGE FILE    ----------------------------

int I3CRenderingEngine::readImageFile(std::fstream *file)
{
    //Delete previous image data
    clearCubesInMemory();

    int iError;

    //Read Cube Side Lenght
    int iSideLenghtUnverified;
    *file >> iSideLenghtUnverified;
    iError = verifyAndAssignSideLength(iSideLenghtUnverified);
    if(iError != I3C_SUCCESS){
        return iError;
    }
    //cout << "Side Lenght: " << m_iSideLength << endl;     // Debug

    m_transform.setUnrotatedCornersCorners(m_iSideLenght);
    setNumberOfLevels();

    //Read Data
    readNumOfMaps(file);
    iError = readCubes(file);
    if(iError != I3C_SUCCESS){
        return iError;
    }

    return I3C_SUCCESS;
}

int I3CRenderingEngine::verifyAndAssignSideLength(int iSideLength)
{
    if(iSideLength <= 1){
        m_iSideLenght = 0;
        return INVALID_CUBE_SIZE;
    }
    else if(!isBase2(iSideLength)){
        m_iSideLenght = 0;
        return SIZE_NOT_BASE_2;
    }
    else{
        m_iSideLenght = iSideLength;
        return I3C_SUCCESS;
    }
}

void I3CRenderingEngine::setNumberOfLevels()
{
    m_iNumberOfLevels = firstHighBit(m_iSideLenght);
    //cout << "Number of Levels: " << m_iNumberOfLevels << endl;    //Debug

    //We assume that we've already made sure that |m_iArrCubeAtLevel| was NULL
    m_iArrCubeAtLevel = new int[m_iNumberOfLevels];
}

void I3CRenderingEngine::readNumOfMaps(std::fstream *file)
{
    int iBuffer = 0;
    m_iTotalNumberOfCubes = 0;

    //Store cubes at each level and count the total number of cubes
    for(int i = 0; i < m_iNumberOfLevels; i++)
    {
        *file >> iBuffer;
        m_iArrCubeAtLevel[i] = iBuffer;
        m_iTotalNumberOfCubes += iBuffer;
    }
    //cout << "Number of Cubes: " << m_iTotalNumberOfCubes << endl;  //Debug

    //TO REMOVE
    //One of the cube is 'this' so we remove 1
    //m_iTotalNumberOfCubes--;

    //Create Cube Pointer Array (we assume that we've made sure that |m_Cubes| was NULL)
    m_Cubes = new I3CCube*[m_iTotalNumberOfCubes];
    for(int i = 0; i < m_iTotalNumberOfCubes; i++){
        m_Cubes[i] = NULL;
    }
}

int I3CRenderingEngine::readCubes(std::fstream *file)
{
    int iError = readPixelCubes(file);
    if(iError != I3C_SUCCESS){
        return iError;
    }
    iError = readIndexCubes(file);
    if(iError != I3C_SUCCESS){
        return iError;
    }

    return I3C_SUCCESS;
}

int I3CRenderingEngine::readPixelCubes(std::fstream *file)
{
    unsigned char ucMap = 0;
    int iBufRedArr[8];
    int iBufGreenArr[8];
    int iBufBlueArr[8];
    int iCubeBeingWritten = 0;
    int iNumOfPixels;
    int iError;

    //Read Pixel Cubes
    for(int i = 0; i < m_iArrCubeAtLevel[0]; i++)
    {
        // Create Cube
        m_Cubes[iCubeBeingWritten] = new I3CPixelCube(m_clTexture, m_clFOV, &m_queue, &m_clCubeDstSorted);
        iError = readMap(file, &ucMap, &iNumOfPixels);
        if(iError != I3C_SUCCESS){
            return iError;
        }

        for(int j = 0; j < iNumOfPixels; j++)
        {
            //Pixel Reading
            *file >> iBufRedArr[j];
            *file >> iBufGreenArr[j];
            *file >> iBufBlueArr[j];
        }

        //TO REMOVE
        /*if(m_iNumberOfLevels == 1){
            this->addPixelsCube(ucMap,
                                iBufRedArr,
                                iBufGreenArr,
                                iBufBlueArr);
        }
        else{*/

        ((I3CPixelCube*)m_Cubes[iCubeBeingWritten])->addPixelsCube(ucMap,
                                                              iBufRedArr,
                                                              iBufGreenArr,
                                                              iBufBlueArr);

        iCubeBeingWritten ++;
    }
    return I3C_SUCCESS;
}

int I3CRenderingEngine::readIndexCubes(std::fstream *file)
{
    unsigned char ucMap = 0;
    int iNumOfChild = 0;
    int iAddressCubesCursorOffset = 0;
    int iCubeBeingWritten = m_iArrCubeAtLevel[0];   //start at index starting right after pixel cubes
    int iError;

    for(int level = 1; level < m_iNumberOfLevels; level++)
    {
        for(int i = 0; i < m_iArrCubeAtLevel[level]; i++)
        {
            iError = readMap(file, &ucMap, &iNumOfChild);
            if(iError != I3C_SUCCESS){
                return iError;
            }

            //Set cube with child addresses
            //TO REMOVE
            /*if(m_iNumberOfLevels == level+1){
                //cout << "Master Cube" << endl;
                //this->addReferenceCube(ucMap, &m_pGVImageArray[iAddressCubesCursorOffset]);
            }
            else{*/

            m_Cubes[iCubeBeingWritten] = new I3CReferenceCube(m_clTexture, m_clFOV, &m_queue,
                                                              m_ucCubeDstSorted, &m_kernelComputeChildCorners);
            ((I3CReferenceCube*)m_Cubes[iCubeBeingWritten])->addReferenceCube(ucMap, &m_Cubes[iAddressCubesCursorOffset]);


            // Update Offset
            iAddressCubesCursorOffset += iNumOfChild;
            iCubeBeingWritten++;
        }
    }

    return I3C_SUCCESS;
}

int I3CRenderingEngine::readMap(fstream *file, unsigned char* ucMap, int* iNumOfPix)
{
    int iBufMap;
    *file >> iBufMap;
    *iNumOfPix = numberHighBits(iBufMap);
    if(*iNumOfPix > 8){
        return FILE_CORRUPTED;
    }
    *ucMap = (unsigned char)iBufMap;

    //Debug
    //cout << "Number of Pixels in Cube: " << *iNumOfPix << endl;
    //cout << "Map: " << iBufMap << endl;

    return I3C_SUCCESS;
}

void I3CRenderingEngine::clearCubesInMemory()
{
    if(m_Cubes != NULL){
        for(int i = 0; i< m_iTotalNumberOfCubes ; i++){
            if(m_Cubes[i] != NULL){
                delete m_Cubes[i];
            }
        }
        delete[] m_Cubes;
        m_Cubes = NULL;
    }
    if(m_iArrCubeAtLevel != NULL){
        delete[] m_iArrCubeAtLevel;
        m_iArrCubeAtLevel = NULL;
    }
}
