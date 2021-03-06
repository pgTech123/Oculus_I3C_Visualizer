/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.cpp
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class manage the GPU/CPU interaction.
 * *********************************************************/

#include "i3crenderingengine.h"

I3CRenderingEngine::I3CRenderingEngine(HDC hDC, HGLRC hRC)
{
    //OpenCL Memory Initialisation
    m_clTexture[0] = NULL;
    m_clTexture[1] = NULL;
    m_clFOV[0] = NULL;
    m_clFOV[1] = NULL;
    m_clRotatedCorners = NULL;
    m_clReferenceCubeMap = NULL;
    m_clPixel = NULL;
    m_clNumOfLevel = NULL;
    m_clBoundingRect = NULL;
    m_clChildId_memStatusBit =NULL;
    m_cltopCubeId = NULL;
    m_clRenderingOrder = NULL;
    m_clDebugOutput = NULL;

    //Program
    m_program = NULL;

    //Kernels
    m_kernelClearCornersComputed = NULL;
    m_kernelRender[0] = NULL;
    m_kernelRender[1] = NULL;

    //Instanciate OpenCL elements
    getOpenGLDevice(hDC, hRC);

    //Image File Initialisation
    m_iArrCubeAtLevel = NULL;
}

I3CRenderingEngine::~I3CRenderingEngine()
{
    //Release Kernels
    if(m_kernelClearCornersComputed != NULL){
        clReleaseKernel(m_kernelClearCornersComputed);
    }
    if(m_kernelRender[0] != NULL){
        clReleaseKernel(m_kernelRender[0]);
    }
    if(m_kernelRender[1] != NULL){
        clReleaseKernel(m_kernelRender[1]);
    }

    //Release Program
    if(m_program != NULL){
        clReleaseProgram(m_program);
    }

    //Release Memory
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
    if(m_clRotatedCorners != NULL){
        clReleaseMemObject(m_clRotatedCorners);
    }
    if(m_clNumOfLevel != NULL){
        clReleaseMemObject(m_clNumOfLevel);
    }
    if(m_clBoundingRect != NULL){
        clReleaseMemObject(m_clBoundingRect);
    }
    if(m_clChildId_memStatusBit != NULL){
        clReleaseMemObject(m_clChildId_memStatusBit);
    }
    if(m_cltopCubeId != NULL){
        clReleaseMemObject(m_cltopCubeId);
    }
    if(m_clRenderingOrder != NULL){
        clReleaseMemObject(m_clRenderingOrder);
    }
    if(m_clDebugOutput != NULL){
        clReleaseMemObject(m_clDebugOutput);
    }

    clearCubesInMemory();

    clReleaseCommandQueue(m_queue);
    clReleaseContext(m_context);
    clReleaseDevice(m_device);
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
    float sinDwn = sin(down);
    float sinUp = sin(up);
    float sinRight = sin(right);
    float sinLeft = sin(left);

    float r_l_ratio = sinLeft / (sinRight + sinLeft);
    float u_d_ratio = sinUp / (sinDwn + sinUp);

    cl_int up_dwn_lft_rght[4];

    //The following values are in pixel.
    up_dwn_lft_rght[0] = (cl_int)((float)m_iHeight[eye] * u_d_ratio);
    up_dwn_lft_rght[1] = (cl_int)(-((float)m_iHeight[eye] - up_dwn_lft_rght[0]));
    up_dwn_lft_rght[2] = (cl_int)(-((float)m_iWidth[eye] * r_l_ratio));
    up_dwn_lft_rght[3] = (cl_int)((float)m_iWidth[eye] + up_dwn_lft_rght[2]);

    //Keep the values in memory
    m_FOV[eye][0] = (int)up_dwn_lft_rght[0];
    m_FOV[eye][1] = (int)up_dwn_lft_rght[1];
    m_FOV[eye][2] = (int)up_dwn_lft_rght[2];
    m_FOV[eye][3] = (int)up_dwn_lft_rght[3];

    //Flush values computed on an OpenCL Buffer
    clEnqueueWriteBuffer(m_queue, m_clFOV[eye], CL_TRUE, 0, 4*sizeof(cl_int), up_dwn_lft_rght, 0, NULL, NULL);
    clSetKernelArg(m_kernelRender[eye], 7, sizeof(m_clFOV[eye]), &m_clFOV[eye]);

}

void I3CRenderingEngine::setTexture(GLuint texId, int eye)
{
    if(eye > 1){    //Eye out of range...
        return;
    }

    //OpenGL memory to OpenCL
    glBindTexture(GL_TEXTURE_2D, texId);
    m_clTexture[eye] = clCreateFromGLTexture(m_context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, texId, NULL);

    //Set Texture Argument
    clSetKernelArg(m_kernelRender[eye], 0, sizeof(m_clTexture[eye]), &m_clTexture[eye]);

    //Get info about the texture
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &m_iWidth[eye]);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &m_iHeight[eye]);
}

void I3CRenderingEngine::setPosition(float x, float y, float z)
{
    m_transform.setTranslation(-x, y, z);
}

void I3CRenderingEngine::setOrientation(float yaw, float pitch, float roll)
{
    m_transform.setAngles(-pitch, yaw, roll);
}

void I3CRenderingEngine::render(int eye)
{
    //Give texture ownership to OpenCL
    glFinish();
    cl_int error = clEnqueueAcquireGLObjects(m_queue, 1,  &m_clTexture[eye], 0, 0, NULL);
    /*if(error != CL_SUCCESS){  //DEBUG
        std::cout << "Aquirering error..." << std::endl;
    }//*/

    //Launch Kernel that clears the previous frame corner positions
    //TODO: Make this call asynchroniously
    size_t workItems[1] = {m_iTotalNumberOfCubes};
    error = clEnqueueNDRangeKernel(m_queue, m_kernelClearCornersComputed, 1, NULL, workItems , NULL, 0, NULL, NULL);
    /*if(error != CL_SUCCESS){      //DEBUG
        std::cout << "Task error clear..." << std::endl;
    }//*/

    //Variable declaration
    float xCornersRotated[8];
    float yCornersRotated[8];
    float zCornersRotated[8];
    cl_float3 cornerRotated[8];
    int minx = m_iWidth[eye];
    int maxx = 0;
    int miny = m_iHeight[eye];
    int maxy = 0;
    cl_int4 boundingRect;
    unsigned char sortedRenderingOrder[8];
    cl_uchar renderingOrder[8];

    //Compute transform of the 8 corners of the image and sort them by distance
    m_transform.computeTransform(xCornersRotated, yCornersRotated, zCornersRotated);
    sort(zCornersRotated, sortedRenderingOrder);

    //Compute bounding rect of the image (relative to the texture)
    for(int i = 0; i < 8; i++){
        //Prepare corners & seq. of rendering to be transfered to GPU memory
        cornerRotated[i].s[0] = (cl_float)xCornersRotated[i];
        cornerRotated[i].s[1] = (cl_float)yCornersRotated[i];
        cornerRotated[i].s[2] = (cl_float)zCornersRotated[i];
        renderingOrder[i] = (cl_uchar)sortedRenderingOrder[i];

        //Computation of bounding rect
        int x = (xCornersRotated[i] / zCornersRotated[i]) - m_FOV[eye][2];
        int y = (yCornersRotated[i]/ zCornersRotated[i]) - m_FOV[eye][1];
        if(x < minx){
            minx = x;
        }
        if(x > maxx){
            maxx = x;
        }
        if(y < miny){
            miny = y;
        }
        if(y > maxy){
            maxy = y;
        }

        //std::cout << (int)renderingOrder[i] << std::endl;     //DEBUG
        //std::cout << xCornersRotated[i] << ", " << yCornersRotated[i] << ", " <<  zCornersRotated[i] << std::endl; //DEBUG
    }

    //Prepare bounding rect to be transfered to GPU memory
    boundingRect.s[0] = (cl_int)minx;
    boundingRect.s[1] = (cl_int)maxx;
    boundingRect.s[2] = (cl_int)miny;
    boundingRect.s[3] = (cl_int)maxy;

    //std::cout << cornerRotated[0].s[0] << std::endl;      //DEBUG
    //std::cout << xCornersRotated[0] << std::endl;         //DEBUG

    //Transfert Corners & Bounding rect & rendering sequence to GPU memory
    clEnqueueWriteBuffer(m_queue, m_clRotatedCorners, CL_TRUE, (m_iTotalNumberOfCubes-1)*8*sizeof(cl_float3),
                         8*sizeof(cl_float3), cornerRotated, 0, NULL, NULL);
    clEnqueueWriteBuffer(m_queue, m_clBoundingRect, CL_TRUE, (m_iTotalNumberOfCubes-1)*sizeof(cl_int4),
                         sizeof(cl_int4), &boundingRect, 0, NULL, NULL);
    clEnqueueWriteBuffer(m_queue, m_clRenderingOrder, CL_TRUE, 0,
                         8*sizeof(cl_uchar), &renderingOrder, 0, NULL, NULL);

    //Launch width x height instance of |render()| kernel
    size_t a[2] = {m_iWidth[eye], m_iHeight[eye]};
    error = clEnqueueNDRangeKernel(m_queue, m_kernelRender[eye], 2, NULL, a , NULL, 0, NULL, NULL);
    /*if(error != CL_SUCCESS){      //DEBUG
        std::cout << "Task error..." << std::endl;
    }//*/

    // DEBUG OUTPUT FROM CL PROGRAM
    //cl_float3 debugResult;
    //clEnqueueReadBuffer(m_queue, m_clDebugOutput, CL_TRUE, 0, sizeof(cl_float3),&debugResult, 0, NULL, NULL);
    //std::cout << "DEBUG: " << debugResult.s[0] << ", " << debugResult.s[1] <<
    //", " << debugResult.s[2] /*<< ", " << debugResult.s[3] */<< std::endl;

    //Give back texture ownership to OpenGL
    clFinish(m_queue);
    error = clEnqueueReleaseGLObjects(m_queue, 1,  &m_clTexture[eye], 0, 0, NULL);
    /*if(error != CL_SUCCESS){      //DEBUG
        std::cout << "Releasing error..." << std::endl;
    }//*/
}

//WARNING: This function is meant to be called only once at the begining (i.e. in the constructor).
//WARNING: No error handeling in this function.
void I3CRenderingEngine::getOpenGLDevice(HDC hDC, HGLRC hRC)
{
    cl_int status;
    cl_uint platAvail;
    cl_uint platformCount = 0;
    cl_platform_id platform[MAX_NUMBER_OF_PLATFORM];

    //Get platforms
    clGetPlatformIDs(0, NULL, &platformCount);
    clGetPlatformIDs(platformCount, platform, NULL);

    for (cl_uint i=0; i < platformCount; i++)
    {
        cl_context_properties props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)platform[i],
                                         CL_GL_CONTEXT_KHR,   (cl_context_properties)hRC,
                                         CL_WGL_HDC_KHR,      (cl_context_properties)hDC, 0};

        //Find device on which OpenGL runs
        clGetGLContextInfoKHR_fn _clGetGLContextInfoKHR = (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
        status = _clGetGLContextInfoKHR(props, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &m_device, NULL);

        if(status == CL_SUCCESS){
            //Create Context and command queue on that device
            m_context = clCreateContext(props, 1, &m_device, 0, 0, NULL);
            m_queue = clCreateCommandQueue(m_context, m_device, CL_QUEUE_PROFILING_ENABLE, NULL);

            //Load kernels and allocate memory
            this->createKernels();
            this->allocateMemory();
            return;
        }
    }
}

void I3CRenderingEngine::createKernels()
{
    Sources_OCL sources = this->loadCLSource("./cl_program.cl");

    if(sources.success){
        //std::cout << "CL Sources found" << std::endl;   //DEBUG

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
        m_kernelClearCornersComputed = clCreateKernel(m_program, "clearMemoryBit", NULL);
        m_kernelRender[0] = clCreateKernel(m_program, "render", NULL);  //Left Texture
        m_kernelRender[1] = clCreateKernel(m_program, "render", NULL);  //Right Texture
    }
    else{
        std::cout << "ERROR: CL Sources NOT found" << std::endl;   //DEBUG
    }
}

void I3CRenderingEngine::allocateMemory()
{
    //TODO: Adjust READ/WRITE rights where needed
    m_clFOV[0] = clCreateBuffer(m_context, CL_MEM_READ_WRITE, 4*sizeof(cl_int), NULL, NULL);
    m_clFOV[1] = clCreateBuffer(m_context, CL_MEM_READ_WRITE, 4*sizeof(cl_int), NULL, NULL);
    m_clNumOfLevel = clCreateBuffer(m_context, CL_MEM_READ_WRITE, sizeof(cl_int), NULL, NULL);
    m_cltopCubeId = clCreateBuffer(m_context, CL_MEM_READ_WRITE, sizeof(cl_int), NULL, NULL);
    m_clRenderingOrder = clCreateBuffer(m_context, CL_MEM_READ_WRITE, 8*sizeof(cl_uchar), NULL, NULL);

    clSetKernelArg(m_kernelRender[0], 9, sizeof(m_clRenderingOrder), &m_clRenderingOrder);
    clSetKernelArg(m_kernelRender[1], 9, sizeof(m_clRenderingOrder), &m_clRenderingOrder);

    //Debug variables
    m_clDebugOutput = clCreateBuffer(m_context, CL_MEM_READ_WRITE, sizeof(cl_float3), NULL, NULL);
    clSetKernelArg(m_kernelRender[0], 10, sizeof(m_clDebugOutput), &m_clDebugOutput);
    clSetKernelArg(m_kernelRender[1], 10, sizeof(m_clDebugOutput), &m_clDebugOutput);

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

    cl_int numOfLevel = (cl_int)m_iNumberOfLevels;
    clEnqueueWriteBuffer(m_queue, m_clNumOfLevel, CL_TRUE, 0, sizeof(numOfLevel), &numOfLevel, 0, NULL, NULL);
    clSetKernelArg(m_kernelRender[0], 1, sizeof(m_clNumOfLevel), &m_clNumOfLevel);
    clSetKernelArg(m_kernelRender[1], 1, sizeof(m_clNumOfLevel), &m_clNumOfLevel);

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

    //Flush |topCubeId| to GPU memory
    cl_int topCubeId = (cl_int)m_iTotalNumberOfCubes;
    clEnqueueWriteBuffer(m_queue, m_cltopCubeId, CL_TRUE, 0, sizeof(topCubeId), &topCubeId, 0, NULL, NULL);
    clSetKernelArg(m_kernelRender[0], 8, sizeof(m_cltopCubeId), &m_cltopCubeId);
    clSetKernelArg(m_kernelRender[1], 8, sizeof(m_cltopCubeId), &m_cltopCubeId);

    //Create Buffer of the right size to store the image data
    m_clReferenceCubeMap = clCreateBuffer(m_context, CL_MEM_READ_WRITE,
                                       m_iTotalNumberOfCubes*sizeof(cl_uchar), NULL, NULL);
    m_clRotatedCorners = clCreateBuffer(m_context, CL_MEM_READ_WRITE,
                                        8*m_iTotalNumberOfCubes*sizeof(cl_float3), NULL, NULL);
    m_clBoundingRect = clCreateBuffer(m_context, CL_MEM_READ_WRITE,
                                        m_iTotalNumberOfCubes*sizeof(cl_int4), NULL, NULL);
    m_clChildId_memStatusBit = clCreateBuffer(m_context, CL_MEM_READ_WRITE,
                                        m_iTotalNumberOfCubes*sizeof(cl_int), NULL, NULL);

    //Set Args
    clSetKernelArg(m_kernelRender[0], 4, sizeof(m_clRotatedCorners), &m_clRotatedCorners);
    clSetKernelArg(m_kernelRender[1], 4, sizeof(m_clRotatedCorners), &m_clRotatedCorners);

    clSetKernelArg(m_kernelRender[0], 5, sizeof(m_clBoundingRect), &m_clBoundingRect);
    clSetKernelArg(m_kernelRender[1], 5, sizeof(m_clBoundingRect), &m_clBoundingRect);

    clSetKernelArg(m_kernelRender[0], 6, sizeof(m_clChildId_memStatusBit), &m_clChildId_memStatusBit);
    clSetKernelArg(m_kernelRender[1], 6, sizeof(m_clChildId_memStatusBit), &m_clChildId_memStatusBit);
    clSetKernelArg(m_kernelClearCornersComputed, 0, sizeof(m_clChildId_memStatusBit), &m_clChildId_memStatusBit);

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
    cl_uchar *map = new cl_uchar[m_iArrCubeAtLevel[0]];
    cl_int *childID = new cl_int[m_iArrCubeAtLevel[0]];

    int iBufRedArr;
    int iBufGreenArr;
    int iBufBlueArr;

    int iPixelOffset = 0;
    int iNumOfPixels;
    int iTotalPixels = 0;

    int iError;

    //Keep a marker in the file
    int marker = file->tellg();

    //Make a first read to know the size
    for(int i = 0; i < m_iArrCubeAtLevel[0]; i++)
    {
        iError = readMap(file, &ucMap, &iNumOfPixels);
        if(iError != I3C_SUCCESS){
            delete[] map;
            delete[] childID;
            return iError;
        }
        for(int j = 0; j < iNumOfPixels; j++)
        {
            //Pixel Reading
            *file >> iBufRedArr;
            *file >> iBufGreenArr;
            *file >> iBufBlueArr;
        }
        iTotalPixels += iNumOfPixels;
    }

    //Allocate the memory needed
    m_clPixel = clCreateBuffer(m_context, CL_MEM_READ_WRITE, iTotalPixels*sizeof(cl_float3), NULL, NULL);
    cl_float3 *pixel = new cl_float3[iTotalPixels];

    //Read a second tmie for data
    file->seekg(marker-8);

    for(int i = 0; i < m_iArrCubeAtLevel[0]; i++)
    {
        iError = readMap(file, &ucMap, &iNumOfPixels);
        if(iError != I3C_SUCCESS){
            delete[] map;
            delete[] childID;
            return iError;
        }

        for(int j = 0; j < iNumOfPixels; j++)
        {
            //Pixel Reading
            *file >> iBufRedArr;
            *file >> iBufGreenArr;
            *file >> iBufBlueArr;

            //Convert to Float3
            pixel[iPixelOffset+j].s[0] = (cl_float)(iBufRedArr)/255;
            pixel[iPixelOffset+j].s[1] = (cl_float)(iBufGreenArr)/255;
            pixel[iPixelOffset+j].s[2] = (cl_float)(iBufBlueArr)/255;
        }
        map[i] = (cl_uchar)ucMap;
        childID[i] = (cl_int)iPixelOffset;

        iPixelOffset += iNumOfPixels;
    }

    //Transfer pixels data and maps of the pixel cubes
    clEnqueueWriteBuffer(m_queue, m_clPixel, CL_TRUE, 0,
                         iTotalPixels*sizeof(cl_float3), pixel, 0, NULL, NULL);
    clEnqueueWriteBuffer(m_queue, m_clReferenceCubeMap, CL_TRUE, 0,
                          m_iArrCubeAtLevel[0]*sizeof(cl_uchar), map, 0, NULL, NULL);
    clEnqueueWriteBuffer(m_queue, m_clChildId_memStatusBit, CL_TRUE, 0,
                          m_iArrCubeAtLevel[0]*sizeof(cl_int), childID, 0, NULL, NULL);

    //Set args
    clSetKernelArg(m_kernelRender[0], 2, sizeof(m_clPixel), &m_clPixel);
    clSetKernelArg(m_kernelRender[1], 2, sizeof(m_clPixel), &m_clPixel);

    //Free memory
    delete[] map;
    delete[] childID;

    return I3C_SUCCESS;
}

int I3CRenderingEngine::readIndexCubes(std::fstream *file)
{
    unsigned char ucMap = 0;
    int iMapToBeWritten = m_iTotalNumberOfCubes - m_iArrCubeAtLevel[0];
    cl_uchar *map = new cl_uchar[iMapToBeWritten];
    cl_int *childID = new cl_int[iMapToBeWritten];

    int index = 0;
    int iOffset = 0;
    int iNumOfChild = 0;
    int iError;

    //Read the maps of each level (except pixel level)
    for(int level = 1; level < m_iNumberOfLevels; level++)
    {
        for(int i = 0; i < m_iArrCubeAtLevel[level]; i++)
        {
            iError = readMap(file, &ucMap, &iNumOfChild);
            if(iError != I3C_SUCCESS){
                delete[] map;
                delete[] childID;
                return iError;
            }

            map[index] = (cl_uchar)ucMap;
            childID[index] = (cl_int)(iOffset | (0x01 << 29));  //offset(ID) + reference bit
            index++;
            iOffset += iNumOfChild;
        }
    }

    //Transfer maps & childID to GPU memory
    clEnqueueWriteBuffer(m_queue, m_clReferenceCubeMap, CL_TRUE, m_iArrCubeAtLevel[0]*sizeof(cl_uchar),
                          iMapToBeWritten*sizeof(cl_uchar), map, 0, NULL, NULL);
    clEnqueueWriteBuffer(m_queue, m_clChildId_memStatusBit, CL_TRUE, m_iArrCubeAtLevel[0]*sizeof(cl_int),
                          iMapToBeWritten*sizeof(cl_int), childID, 0, NULL, NULL);

    //Set Args
    clSetKernelArg(m_kernelRender[0], 3, sizeof(m_clReferenceCubeMap), &m_clReferenceCubeMap);
    clSetKernelArg(m_kernelRender[1], 3, sizeof(m_clReferenceCubeMap), &m_clReferenceCubeMap);

    //Free memory
    delete[] map;
    delete[] childID;

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
    if(m_iArrCubeAtLevel != NULL){
        delete[] m_iArrCubeAtLevel;
        m_iArrCubeAtLevel = NULL;
    }

    //Clear OpenCL buffers
    if(m_clReferenceCubeMap != NULL){
        clReleaseMemObject(m_clReferenceCubeMap);
    }
    if(m_clPixel != NULL){
        clReleaseMemObject(m_clPixel);
    }
}
