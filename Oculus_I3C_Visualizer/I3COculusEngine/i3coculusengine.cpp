/*************************************************************************
 * Project   :      Oculus_I3C_Visualizer
 * Filename  :      i3coculusengine.cpp
 * Author    :      Pascal Gendron
 * Version   :      0.1
 *
 * Copyright :      GNU GENERAL PUBLIC LICENSE
 * ************************************************************************/

#include "i3coculusengine.h"

I3COculusEngine::I3COculusEngine()
{
    m_pucData = NULL;
    m_pucPixelsFilled = NULL;
    m_iArrCubeAtLevel = NULL;
    m_pGVImageArray = NULL;
}

I3COculusEngine::~I3COculusEngine()
{
    clearImageInMemory();
}

int I3COculusEngine::openI3CFile(const char* filename)
{
    fstream file;

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

bool I3COculusEngine::setImageSize(int width, int height)
{
    if(m_pucData == NULL && m_pucPixelsFilled == NULL){
        m_width = width;
        m_height = height;
        int imageDataSize = m_width * m_height * 3;
        m_pucData = new unsigned char[imageDataSize];
        m_pucPixelsFilled = new unsigned char[m_width * m_height];
        return true;
    }
    return false;
}

void I3COculusEngine::setFOV(float down, float up, float right, float left)
{
    m_Transform.setFOV(down, up, right, left);
}

void I3COculusEngine::setRotation(double yaw, double pitch, double roll)
{
    m_Transform.setAngles(yaw, pitch, roll);
}

void I3COculusEngine::setPosition(double x, double y, double z)
{
    m_Transform.setTranslation(x, y, z);
}

void I3COculusEngine::generateImage()
{
    int width_x_height = m_width * m_height;
    //Initialize every pixels as empty
    for(int i = 0; i < width_x_height; i++)
    {
        m_pucPixelsFilled[i] = 0;
    }

    //Compute cube corners projected on the frame
    m_Transform.computeTransform(m_dScreenTransformedCornerX,
                                 m_dScreenTransformedCornerY,
                                 m_dDstFromScreenTransformed);

    //Sort points on Z axis by distance
    sort(m_dDstFromScreenTransformed, m_dCornerSortedByDst);

    for(int i = 0; i < 8; i++){
        cout << "Rotated Corner " << i << " X : " << m_dScreenTransformedCornerX[i] << endl;
        cout << "Rotated Corner " << i << " Y : " << m_dScreenTransformedCornerY[i] << endl;
        cout << "Rotated Corner " << i << " Z : " << m_dDstFromScreenTransformed[i] << endl;
        cout << "Corner Order: " << i << " : " << (int)m_dCornerSortedByDst[i] << endl;
    }//*/

    //Rendering
    //m_Transform.getImageCenterPoint(&m_iCenterPointX, &m_iCenterPointY);
    /*ApplyRotation_and_Render(m_dScreenTransformedCornerX,
                             m_dScreenTransformedCornerY,
                             m_dCornerSortedByDst,
                             (float)m_iCenterPointX,
                             (float)m_iCenterPointY);*/

    //TODO: Faire comme du monde
    RenderingScreen scr;
    scr.left_rightRatio = 0.4;
    scr.up_downRatio = 0.7;
    scr.focalLength = 300;

    render(m_dScreenTransformedCornerX,
           m_dScreenTransformedCornerY,
           m_dDstFromScreenTransformed,
           &scr,
           m_dCornerSortedByDst);

    //Fill every pixels left empty with black
    for(int i = 0; i < width_x_height; i++)
    {
        if(m_pucPixelsFilled[i] == 0)
        {
            m_pucData[3*i] = 0;
            m_pucData[(3*i) + 1] = 0;
            m_pucData[(3*i) + 2] = 0;
        }
    }

    //TODO: consider distortion while rendering
}

unsigned char* I3COculusEngine::getData()
{
    return m_pucData;
}

int I3COculusEngine::readImageFile(fstream *file)
{
    //Delete previous image data
    //clearImageInMemory();

    int iError;

    //Read Cube Side Lenght
    int iSideLenghtUnverified;
    *file >> iSideLenghtUnverified;
    iError = verifyAndAssignSideLength(iSideLenghtUnverified);
    if(iError != NO_ERRORS){
        return iError;
    }
    //cout << "Side Lenght: " << m_iSideLength << endl;     // Debug

    //Preparing to Read Image
    setImageCenterPoint();
    //TODO: Consider pixel density: now 1px = 1mm
    m_Transform.setUnrotatedCornersCorners(m_iCenterPointX, m_iCenterPointY, m_iSideLength);
    setNumberOfLevels();

    /* Read Data */
    readNumOfMaps(file);
    iError = readCubes(file);
    if(iError != NO_ERRORS){
        return iError;
    }

    return NO_ERRORS;
}

int I3COculusEngine::verifyAndAssignSideLength(int iSideLength)
{
    if(iSideLength <= 1){
        m_iSideLength = 0;
        return INVALID_CUBE_SIZE;
    }
    else if(!isBase2(iSideLength)){
        m_iSideLength = 0;
        return SIZE_NOT_BASE_2;
    }
    else{
        m_iSideLength = iSideLength;
        return NO_ERRORS;
    }
}

void I3COculusEngine::setImageCenterPoint()
{
    m_iCenterPointX = m_width/2;
    m_iCenterPointY = m_height/2;
}

void I3COculusEngine::setNumberOfLevels()
{
    m_iNumberOfLevels = firstHighBit(m_iSideLength);
    //cout << "Number of Levels: " << m_iNumberOfLevels << endl;    //Debug
    m_iArrCubeAtLevel = new int[m_iNumberOfLevels];
}

void I3COculusEngine::readNumOfMaps(fstream *file)
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

    //One of the cube is 'this' so we remove 1
    m_iTotalNumberOfCubes--;

    //Create Cube Pointer Array
    m_pGVImageArray = new I3CCube*[m_iTotalNumberOfCubes];
}

int I3COculusEngine::readCubes(fstream *file)
{
    int iError = readPixelCubes(file);
    if(iError != NO_ERRORS){
        return iError;
    }
    iError = readIndexCubes(file);
    if(iError != NO_ERRORS){
        return iError;
    }

    return NO_ERRORS;
}

int I3COculusEngine::readPixelCubes(fstream *file)
{
    unsigned char ucMap = 0;
    int iBufRedArr[8];
    int iBufGreenArr[8];
    int iBufBlueArr[8];
    int iCubeBeingWritten = 0;
    int iNumOfPixels;
    int iError;

    /* Read Pixel Cubes */
    for(int i = 0; i < m_iArrCubeAtLevel[0]; i++)
    {
        /* Create Cube */
        m_pGVImageArray[iCubeBeingWritten] = new I3CCube(&m_width,
                                                             &m_height,
                                                             &*m_pucData,
                                                             &*m_pucPixelsFilled);
        iError = readMap(file, &ucMap, &iNumOfPixels);
        if(iError != NO_ERRORS){
            return iError;
        }

        for(int j = 0; j < iNumOfPixels; j++)
        {
            //PIXEL READING
            *file >> iBufRedArr[j];
            *file >> iBufGreenArr[j];
            *file >> iBufBlueArr[j];
        }

        if(m_iNumberOfLevels == 1){
            this->addPixelsCube(ucMap,
                                iBufRedArr,
                                iBufGreenArr,
                                iBufBlueArr);
        }
        else{
            m_pGVImageArray[iCubeBeingWritten]->addPixelsCube(ucMap,
                                                              iBufRedArr,
                                                              iBufGreenArr,
                                                              iBufBlueArr);
        }

        iCubeBeingWritten ++;
    }
    return NO_ERRORS;
}

int I3COculusEngine::readIndexCubes(fstream *file)
{
    unsigned char ucMap = 0;
    int iNumOfChild = 0;
    int iAddressCubesCursorOffset = 0;
    int iCubeBeingWritten = m_iArrCubeAtLevel[0];
    int iError;

    for(int level = 1; level < m_iNumberOfLevels; level++)
    {
        for(int i = 0; i < m_iArrCubeAtLevel[level]; i++)
        {
            iError = readMap(file, &ucMap, &iNumOfChild);
            if(iError != NO_ERRORS){
                return iError;
            }

            /* Set cube with child addresses */
            if(m_iNumberOfLevels == level+1){
                //cout << "Master Cube" << endl;
                this->addReferenceCube(ucMap, &m_pGVImageArray[iAddressCubesCursorOffset]);
            }
            else{

                m_pGVImageArray[iCubeBeingWritten] = new I3CCube(&m_width,
                                                                     &m_height,
                                                                     &*m_pucData,
                                                                     &*m_pucPixelsFilled);

                m_pGVImageArray[iCubeBeingWritten]->addReferenceCube(ucMap,
                                                                     &m_pGVImageArray[iAddressCubesCursorOffset]);
            }

            /* Update Offset */
            iAddressCubesCursorOffset += iNumOfChild;
            iCubeBeingWritten++;
        }
    }

    return NO_ERRORS;
}

int I3COculusEngine::readMap(fstream *file, unsigned char* ucMap, int* iNumOfPix)
{
    int iBufMap;
    *file >> iBufMap;
    *iNumOfPix = numberHighBits(iBufMap);
    if(*iNumOfPix > 8){
        return FILE_CORRUPTED;
    }
    *ucMap = (unsigned char)iBufMap;

    /* Debug */
    //cout << "Number of Pixels in Cube: " << *iNumOfPix << endl;
    //cout << "Map: " << iBufMap << endl;

    return NO_ERRORS;
}

void I3COculusEngine::clearImageInMemory()
{
    if(m_pucData != NULL){
        delete[] m_pucData;
        m_pucData = NULL;
    }
    if(m_pucPixelsFilled != NULL){
        delete[] m_pucPixelsFilled;
        m_pucPixelsFilled = NULL;
    }

 /*   if(m_pGVImageArray != NULL){
        for(int i = 0; i< m_iTotalNumberOfCubes ; i++){
            delete m_pGVImageArray[i];
        }
        delete[] m_pGVImageArray;
        m_pGVImageArray = NULL;
    }
*/
    if(m_iArrCubeAtLevel != NULL){
        delete[] m_iArrCubeAtLevel;
        m_iArrCubeAtLevel = NULL;
    }
}

