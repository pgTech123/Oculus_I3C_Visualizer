/*************************************************************************
 * Project   :      Oculus_I3C_Visualizer
 * Filename  :      i3cindexcube.cpp
 * Author    :      Pascal Gendron
 * Version   :      0.1
 *
 * Copyright :      GNU GENERAL PUBLIC LICENSE
 * ************************************************************************/

//TODO: Clean code
#include "i3cindexcube.h"

GVIndexCube::GVIndexCube()
{
    initializeCube();
}

GVIndexCube::GVIndexCube(int* p_iImageWidth, int* p_iImageHeight, unsigned char* p_ucImageData,
                         bool* p_bPixelFilled)
{
    /* Preset everithing to 0 then try to charge specified data */
    initializeCube();    //TODO: Catch possible error
    setImageProperty(p_iImageWidth, p_iImageHeight, p_ucImageData, p_bPixelFilled);
}
GVIndexCube::~GVIndexCube()
{

    if(m_iHierarchyLevel == 0){
        delete[] m_ucBlue;
        delete[] m_ucGreen;
        delete[] m_ucRed;
    }
    else if(m_iHierarchyLevel > 0){
        delete[] m_p_GVIndexCubeArray;
    }


}

void GVIndexCube::initializeCube()
{
    m_p_iImageWidth = NULL;
    m_p_iImageHeight = NULL;
    m_p_ucImageData = NULL;
    m_p_bPixelFilled = NULL;
    m_iHierarchyLevel = -1;
}

bool GVIndexCube::setImageProperty(int* p_iImageWidth, int* p_iImageHeight,
                                   unsigned char* p_ucImageData,  bool* p_bPixelFilled)
{
    if(*p_iImageWidth != 0 && *p_iImageHeight != 0 && p_ucImageData != NULL && p_bPixelFilled != NULL){
        m_p_iImageWidth = p_iImageWidth;
        m_p_iImageHeight = p_iImageHeight;
        m_p_ucImageData = p_ucImageData;
        m_p_bPixelFilled = p_bPixelFilled;

        return true;
    }
    else{
        return false;
    }
}

int GVIndexCube::getHierarchyLevel()
{
    return m_iHierarchyLevel;
}

void GVIndexCube::addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue)
{
    m_ucMap = ucMap;
    m_iHierarchyLevel = 0;
    int i_Counter = 0;

    /* Create storage according to the map */
    m_ucRed = new unsigned char[8];
    m_ucGreen = new unsigned char[8];
    m_ucBlue = new unsigned char[8];

    /* Set the value for each specified pixel */
    for(int i = 0; i < 8; i++)
    {
        if(m_ucMap & (0x01 << i)){
            m_ucRed[i] = (unsigned char)ucRed[i_Counter];
            m_ucGreen[i] = (unsigned char)ucGreen[i_Counter];
            m_ucBlue[i] = (unsigned char)ucBlue[i_Counter];
            i_Counter++;
        }
    }
}

void GVIndexCube::addReferenceCube(unsigned char ucMap, GVIndexCube** p_ChildCubeRef)
{
    /* Set hierarchy level */
    m_iHierarchyLevel = p_ChildCubeRef[0]->getHierarchyLevel() + 1;

    /* Create storage according to the map */
    m_ucMap = ucMap;
    m_p_GVIndexCubeArray = new GVIndexCube*[8];

    /* Set the reference for each specified child cube */
    int i_Counter = 0;
    for(int i = 0; i < 8; i++)
    {
        if(m_ucMap & (0x01 << i)){
            m_p_GVIndexCubeArray[i] = (GVIndexCube*)p_ChildCubeRef[i_Counter];
            i_Counter++;
        }
    }
}

void GVIndexCube::ApplyRotation_and_Render( float iArrPosXRotation[8], //relative
                                            float iArrPosYRotation[8], //relative
                                            unsigned char ucSortedByDstFromScreen[8],
                                            float dCenterPointX,
                                            float dCenterPointY)
{
    /* Render cubes accordingly to their level */
    if(m_iHierarchyLevel > 0){
        renderReference(iArrPosXRotation,
                    iArrPosYRotation,
                    ucSortedByDstFromScreen,
                    dCenterPointX,
                    dCenterPointY);
    }
    else{
        renderPixel(iArrPosXRotation,
                    iArrPosYRotation,
                    ucSortedByDstFromScreen,
                    dCenterPointX,
                    dCenterPointY);
    }
}

void GVIndexCube::renderReference(float dArrPosXRotation[8],
                    float dArrPosYRotation[8],
                    unsigned char ucSortedByDstFromScreen[8],
                    float dCenterPointX,
                    float dCenterPointY)
{
    /* Find Array Mid Point */
    float dMidArrX[12];
    float dMidArrY[12];
    computeMidArr(dArrPosXRotation,
                  dArrPosYRotation,
                  dMidArrX,
                  dMidArrY);

    /* Find Face Mid Point */
    float dMidFaceXArr[6];
    float dMidFaceYArr[6];
    computeMidFace(dMidArrX,
                   dMidArrY,
                   dMidFaceXArr,
                   dMidFaceYArr);

    for(int dst = 0; dst < 8; dst ++)
    {
        if((m_ucMap & (0x01 << ucSortedByDstFromScreen[dst])))
        {
            /* Compute child corners */
            computeChildCorners(dArrPosXRotation,
                           dArrPosYRotation,
                           ucSortedByDstFromScreen[dst],
                           dCenterPointX,
                           dCenterPointY,
                           dMidArrX,
                           dMidArrY,
                           dMidFaceXArr,
                           dMidFaceYArr);

            /* Verify if child is suseptible to be written */
            if(!isChildFullyHidden())
            {
                /* Rotate and Render Child */
                m_p_GVIndexCubeArray[ucSortedByDstFromScreen[dst]]->ApplyRotation_and_Render(m_dChildComputedCornersX,
                                                                                             m_dChildComputedCornersY,
                                                                                             ucSortedByDstFromScreen,//TODO: verify mathematically that this is correct
                                                                                             m_dChildCenterPointX,
                                                                                             m_dChildCenterPointY);//*/
            }
        }
    }
}

void GVIndexCube::computeMidArr(float* dArrPosXRotation,
                   float* dArrPosYRotation,
                   float* dMidArrX,
                   float* dMidArrY)
{
    //TO CHANGE WHEN PERSECTIVE
    dMidArrX[0] = (dArrPosXRotation[0]+dArrPosXRotation[1])/2;
    dMidArrY[0] = (dArrPosYRotation[0]+dArrPosYRotation[1])/2;
    dMidArrX[1] = (dArrPosXRotation[1]+dArrPosXRotation[2])/2;
    dMidArrY[1] = (dArrPosYRotation[1]+dArrPosYRotation[2])/2;
    dMidArrX[2] = (dArrPosXRotation[2]+dArrPosXRotation[3])/2;
    dMidArrY[2] = (dArrPosYRotation[2]+dArrPosYRotation[3])/2;
    dMidArrX[3] = (dArrPosXRotation[3]+dArrPosXRotation[0])/2;
    dMidArrY[3] = (dArrPosYRotation[3]+dArrPosYRotation[0])/2;
    dMidArrX[4] = (dArrPosXRotation[0]+dArrPosXRotation[4])/2;
    dMidArrY[4] = (dArrPosYRotation[0]+dArrPosYRotation[4])/2;
    dMidArrX[5] = (dArrPosXRotation[1]+dArrPosXRotation[5])/2;
    dMidArrY[5] = (dArrPosYRotation[1]+dArrPosYRotation[5])/2;
    dMidArrX[6] = (dArrPosXRotation[2]+dArrPosXRotation[6])/2;
    dMidArrY[6] = (dArrPosYRotation[2]+dArrPosYRotation[6])/2;
    dMidArrX[7] = (dArrPosXRotation[3]+dArrPosXRotation[7])/2;
    dMidArrY[7] = (dArrPosYRotation[3]+dArrPosYRotation[7])/2;
    dMidArrX[8] = (dArrPosXRotation[4]+dArrPosXRotation[5])/2;
    dMidArrY[8] = (dArrPosYRotation[4]+dArrPosYRotation[5])/2;
    dMidArrX[9] = (dArrPosXRotation[5]+dArrPosXRotation[6])/2;
    dMidArrY[9] = (dArrPosYRotation[5]+dArrPosYRotation[6])/2;
    dMidArrX[10] = (dArrPosXRotation[6]+dArrPosXRotation[7])/2;
    dMidArrY[10] = (dArrPosYRotation[6]+dArrPosYRotation[7])/2;
    dMidArrX[11] = (dArrPosXRotation[7]+dArrPosXRotation[4])/2;
    dMidArrY[11] = (dArrPosYRotation[7]+dArrPosYRotation[4])/2;
}

void GVIndexCube::computeMidFace(float *dMidArrX,
                                 float *dMidArrY,
                                 float *dMidFaceXArr,
                                 float *dMidFaceYArr)
{
    //TO CHANGE WHEN PERSECTIVE
    dMidFaceXArr[0] = (dMidArrX[0]+dMidArrX[2])/2;
    dMidFaceYArr[0] = (dMidArrY[0]+dMidArrY[2])/2;
    dMidFaceXArr[1] = (dMidArrX[0]+dMidArrX[8])/2;
    dMidFaceYArr[1] = (dMidArrY[0]+dMidArrY[8])/2;
    dMidFaceXArr[2] = (dMidArrX[1]+dMidArrX[9])/2;
    dMidFaceYArr[2] = (dMidArrY[1]+dMidArrY[9])/2;
    dMidFaceXArr[3] = (dMidArrX[2]+dMidArrX[10])/2;
    dMidFaceYArr[3] = (dMidArrY[2]+dMidArrY[10])/2;
    dMidFaceXArr[4] = (dMidArrX[3]+dMidArrX[11])/2;
    dMidFaceYArr[4] = (dMidArrY[3]+dMidArrY[11])/2;
    dMidFaceXArr[5] = (dMidArrX[8]+dMidArrX[10])/2;
    dMidFaceYArr[5] = (dMidArrY[8]+dMidArrY[10])/2;
}

void GVIndexCube::computeChildCorners(float* dArrPosXRotation,
                                      float* dArrPosYRotation,
                                      unsigned char ucMapIndex,
                                      float dCenterPointX,
                                      float dCenterPointY,
                                      float* dMidArrX,
                                      float* dMidArrY,
                                      float* dMidFaceXArr,
                                      float* dMidFaceYArr)
{
    //ucMapIndex => 0 to 7
    if(ucMapIndex == 0){
        m_dChildComputedCornersX[0] = dArrPosXRotation[0];
        m_dChildComputedCornersY[0] = dArrPosYRotation[0];
        m_dChildComputedCornersX[1] = dMidArrX[0];
        m_dChildComputedCornersY[1] = dMidArrY[0];
        m_dChildComputedCornersX[2] = dMidFaceXArr[0];
        m_dChildComputedCornersY[2] = dMidFaceYArr[0];
        m_dChildComputedCornersX[3] = dMidArrX[3];
        m_dChildComputedCornersY[3] = dMidArrY[3];
        m_dChildComputedCornersX[4] = dMidArrX[4];
        m_dChildComputedCornersY[4] = dMidArrY[4];
        m_dChildComputedCornersX[5] = dMidFaceXArr[1];
        m_dChildComputedCornersY[5] = dMidFaceYArr[1];
        m_dChildComputedCornersX[6] = dCenterPointX;
        m_dChildComputedCornersY[6] = dCenterPointY;
        m_dChildComputedCornersX[7] = dMidFaceXArr[4];
        m_dChildComputedCornersY[7] = dMidFaceYArr[4];
    }
    else if(ucMapIndex == 1){
        m_dChildComputedCornersX[0] = dMidArrX[0];
        m_dChildComputedCornersY[0] = dMidArrY[0];
        m_dChildComputedCornersX[1] = dArrPosXRotation[1];
        m_dChildComputedCornersY[1] = dArrPosYRotation[1];
        m_dChildComputedCornersX[2] = dMidArrX[1];
        m_dChildComputedCornersY[2] = dMidArrY[1];
        m_dChildComputedCornersX[3] = dMidFaceXArr[0];
        m_dChildComputedCornersY[3] = dMidFaceYArr[0];
        m_dChildComputedCornersX[4] = dMidFaceXArr[1];
        m_dChildComputedCornersY[4] = dMidFaceYArr[1];
        m_dChildComputedCornersX[5] = dMidArrX[5];
        m_dChildComputedCornersY[5] = dMidArrY[5];
        m_dChildComputedCornersX[6] = dMidFaceXArr[2];
        m_dChildComputedCornersY[6] = dMidFaceYArr[2];
        m_dChildComputedCornersX[7] = dCenterPointX;
        m_dChildComputedCornersY[7] = dCenterPointY;
    }
    else if(ucMapIndex == 2){
        m_dChildComputedCornersX[0] = dMidFaceXArr[0];
        m_dChildComputedCornersY[0] = dMidFaceYArr[0];
        m_dChildComputedCornersX[1] = dMidArrX[1];
        m_dChildComputedCornersY[1] = dMidArrY[1];
        m_dChildComputedCornersX[2] = dArrPosXRotation[2];
        m_dChildComputedCornersY[2] = dArrPosYRotation[2];
        m_dChildComputedCornersX[3] = dMidArrX[2];
        m_dChildComputedCornersY[3] = dMidArrY[2];
        m_dChildComputedCornersX[4] = dCenterPointX;
        m_dChildComputedCornersY[4] = dCenterPointY;
        m_dChildComputedCornersX[5] = dMidFaceXArr[2];
        m_dChildComputedCornersY[5] = dMidFaceYArr[2];
        m_dChildComputedCornersX[6] = dMidArrX[6];
        m_dChildComputedCornersY[6] = dMidArrY[6];
        m_dChildComputedCornersX[7] = dMidFaceXArr[3];
        m_dChildComputedCornersY[7] = dMidFaceYArr[3];
    }
    else if(ucMapIndex == 3){
        m_dChildComputedCornersX[0] = dMidArrX[3];
        m_dChildComputedCornersY[0] = dMidArrY[3];
        m_dChildComputedCornersX[1] = dMidFaceXArr[0];
        m_dChildComputedCornersY[1] = dMidFaceYArr[0];
        m_dChildComputedCornersX[2] = dMidArrX[2];
        m_dChildComputedCornersY[2] = dMidArrY[2];
        m_dChildComputedCornersX[3] = dArrPosXRotation[3];
        m_dChildComputedCornersY[3] = dArrPosYRotation[3];
        m_dChildComputedCornersX[4] = dMidFaceXArr[4];
        m_dChildComputedCornersY[4] = dMidFaceYArr[4];
        m_dChildComputedCornersX[5] = dCenterPointX;
        m_dChildComputedCornersY[5] = dCenterPointY;
        m_dChildComputedCornersX[6] = dMidFaceXArr[3];
        m_dChildComputedCornersY[6] = dMidFaceYArr[3];
        m_dChildComputedCornersX[7] = dMidArrX[7];
        m_dChildComputedCornersY[7] = dMidArrY[7];
    }
    else if(ucMapIndex == 4){
        m_dChildComputedCornersX[0] = dMidArrX[4];
        m_dChildComputedCornersY[0] = dMidArrY[4];
        m_dChildComputedCornersX[1] = dMidFaceXArr[1];
        m_dChildComputedCornersY[1] = dMidFaceYArr[1];
        m_dChildComputedCornersX[2] = dCenterPointX;
        m_dChildComputedCornersY[2] = dCenterPointY;
        m_dChildComputedCornersX[3] = dMidFaceXArr[4];
        m_dChildComputedCornersY[3] = dMidFaceYArr[4];
        m_dChildComputedCornersX[4] = dArrPosXRotation[4];
        m_dChildComputedCornersY[4] = dArrPosYRotation[4];
        m_dChildComputedCornersX[5] = dMidArrX[8];
        m_dChildComputedCornersY[5] = dMidArrY[8];
        m_dChildComputedCornersX[6] = dMidFaceXArr[5];
        m_dChildComputedCornersY[6] = dMidFaceYArr[5];
        m_dChildComputedCornersX[7] = dMidArrX[11];
        m_dChildComputedCornersY[7] = dMidArrY[11];
    }
    else if(ucMapIndex == 5){
        m_dChildComputedCornersX[0] = dMidFaceXArr[1];
        m_dChildComputedCornersY[0] = dMidFaceYArr[1];
        m_dChildComputedCornersX[1] = dMidArrX[5];
        m_dChildComputedCornersY[1] = dMidArrY[5];
        m_dChildComputedCornersX[2] = dMidFaceXArr[2];
        m_dChildComputedCornersY[2] = dMidFaceYArr[2];
        m_dChildComputedCornersX[3] = dCenterPointX;
        m_dChildComputedCornersY[3] = dCenterPointY;
        m_dChildComputedCornersX[4] = dMidArrX[8];
        m_dChildComputedCornersY[4] = dMidArrY[8];
        m_dChildComputedCornersX[5] = dArrPosXRotation[5];
        m_dChildComputedCornersY[5] = dArrPosYRotation[5];
        m_dChildComputedCornersX[6] = dMidArrX[9];
        m_dChildComputedCornersY[6] = dMidArrY[9];
        m_dChildComputedCornersX[7] = dMidFaceXArr[5];
        m_dChildComputedCornersY[7] = dMidFaceYArr[5];
    }
    else if(ucMapIndex == 6){
        m_dChildComputedCornersX[0] = dCenterPointX;
        m_dChildComputedCornersY[0] = dCenterPointY;
        m_dChildComputedCornersX[1] = dMidFaceXArr[2];
        m_dChildComputedCornersY[1] = dMidFaceYArr[2];
        m_dChildComputedCornersX[2] = dMidArrX[6];
        m_dChildComputedCornersY[2] = dMidArrY[6];
        m_dChildComputedCornersX[3] = dMidFaceXArr[3];
        m_dChildComputedCornersY[3] = dMidFaceYArr[3];
        m_dChildComputedCornersX[4] = dMidFaceXArr[5];
        m_dChildComputedCornersY[4] = dMidFaceYArr[5];
        m_dChildComputedCornersX[5] = dMidArrX[9];
        m_dChildComputedCornersY[5] = dMidArrY[9];
        m_dChildComputedCornersX[6] = dArrPosXRotation[6];
        m_dChildComputedCornersY[6] = dArrPosYRotation[6];
        m_dChildComputedCornersX[7] = dMidArrX[10];
        m_dChildComputedCornersY[7] = dMidArrY[10];
    }
    else if(ucMapIndex == 7){
        m_dChildComputedCornersX[0] = dMidFaceXArr[4];
        m_dChildComputedCornersY[0] = dMidFaceYArr[4];
        m_dChildComputedCornersX[1] = dCenterPointX;
        m_dChildComputedCornersY[1] = dCenterPointY;
        m_dChildComputedCornersX[2] = dMidFaceXArr[3];
        m_dChildComputedCornersY[2] = dMidFaceYArr[3];
        m_dChildComputedCornersX[3] = dMidArrX[7];
        m_dChildComputedCornersY[3] = dMidArrY[7];
        m_dChildComputedCornersX[4] = dMidArrX[11];
        m_dChildComputedCornersY[4] = dMidArrY[11];
        m_dChildComputedCornersX[5] = dMidFaceXArr[5];
        m_dChildComputedCornersY[5] = dMidFaceYArr[5];
        m_dChildComputedCornersX[6] = dMidArrX[10];
        m_dChildComputedCornersY[6] = dMidArrY[10];
        m_dChildComputedCornersX[7] = dArrPosXRotation[7];
        m_dChildComputedCornersY[7] = dArrPosYRotation[7];
    }
    else{
        return;
    }
    findCenterPoint();
}

void GVIndexCube::findCenterPoint()
{
    float dMidVerticalX[2];
    float dMidVerticalY[2];
    dMidVerticalX[0] = (m_dChildComputedCornersX[0]+m_dChildComputedCornersX[4])/2;
    dMidVerticalY[0] = (m_dChildComputedCornersY[0]+m_dChildComputedCornersY[4])/2;
    dMidVerticalX[1] = (m_dChildComputedCornersX[2]+m_dChildComputedCornersX[6])/2;
    dMidVerticalY[1] = (m_dChildComputedCornersY[2]+m_dChildComputedCornersY[6])/2;

    m_dChildCenterPointX = (dMidVerticalX[0] + dMidVerticalX[1])/2;
    m_dChildCenterPointY = (dMidVerticalY[0] + dMidVerticalY[1])/2;
}

bool GVIndexCube::isChildFullyHidden()
{
    /* Scan to see if any pixel in the region defined by m_dChildComputedCornersX
     * and m_dChildComputedCornersY is not written*/
    /*
    int minValueX =
    int minValueY =
    int maxValueX =
    int maxValueY =
    */

    //TODO: Check in square if everithing is defined

    return false;
}

void GVIndexCube::renderPixel(float iArrPosXRotation[8],
                    float iArrPosYRotation[8],
                    unsigned char ucSortedByDstFromScreen[8],
                    float dCenterPointX,
                    float dCenterPointY)
{
    /* Round center point */
    int iCenterPointXRounded = (int)(dCenterPointX);    //(int)round(dCenterPointX);
    int iCenterPointYRounded = (int)(dCenterPointY);    //(int)round(dCenterPointY);

    /* Render pixels in order */
    for(int i = 0; i < 8; i++)
    {
        /* Debug */
        //cout << " posX: "<<i <<"  "<<iArrPosXRotation[i] << endl;
        //cout << " posY: "<<i <<"  "<<iArrPosYRotation[i] << endl;

        /* If pixel exist */
        if((m_ucMap & (0x01 << ucSortedByDstFromScreen[i])))
        {
            //TODO: Condition if not in the image
            if(iArrPosXRotation[ucSortedByDstFromScreen[i]] < iCenterPointXRounded)
            {

                if(iArrPosYRotation[ucSortedByDstFromScreen[i]] < iCenterPointYRounded)
                {
                    /* Pixel Position */
                    //  ---------
                    //  | x |   |
                    //  ---------       center: centerPointRounded
                    //  |   |   |
                    //  ---------
                    /* Verify if not written yet */
                    if(!m_p_bPixelFilled[(iCenterPointXRounded-1)+((iCenterPointYRounded-1)*(*m_p_iImageWidth))])
                    {
                        /* Write pixel */
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3] = m_ucRed[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3+1] = m_ucGreen[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3+2] = m_ucBlue[ucSortedByDstFromScreen[i]];
                        m_p_bPixelFilled[(iCenterPointXRounded-1)+((iCenterPointYRounded-1)*(*m_p_iImageWidth))] = true;
                    }
                }
                else
                {
                    /* Pixel Position */
                    //  ---------
                    //  |   |   |
                    //  ---------       center: centerPointRounded
                    //  | x |   |
                    //  ---------
                    /* Verify if not written yet */
                    if(!m_p_bPixelFilled[(iCenterPointXRounded-1)+((iCenterPointYRounded)*(*m_p_iImageWidth))])
                    {
                        /* Write pixel */
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded)*(*m_p_iImageWidth))*3] = m_ucRed[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded)*(*m_p_iImageWidth))*3+1] = m_ucGreen[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded-1+(iCenterPointYRounded)*(*m_p_iImageWidth))*3+2] = m_ucBlue[ucSortedByDstFromScreen[i]];
                        m_p_bPixelFilled[(iCenterPointXRounded-1)+((iCenterPointYRounded)*(*m_p_iImageWidth))] = true;
                    }
                }
            }
            else
            {

                if(iArrPosYRotation[ucSortedByDstFromScreen[i]] < iCenterPointYRounded)
                {

                    /* Pixel Position */
                    //  ---------
                    //  |   |x  |
                    //  ---------       center: centerPointRounded
                    //  |   |   |
                    //  ---------
                    /* Verify if not written yet */
                    if(!m_p_bPixelFilled[(iCenterPointXRounded)+((iCenterPointYRounded-1)*(*m_p_iImageWidth))])
                    {
                        /* Write pixel */
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3] = m_ucRed[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3+1] = m_ucGreen[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded-1)*(*m_p_iImageWidth))*3+2] = m_ucBlue[ucSortedByDstFromScreen[i]];
                        m_p_bPixelFilled[(iCenterPointXRounded)+((iCenterPointYRounded-1)*(*m_p_iImageWidth))] = true;
                    }
                }
                else
                {
                    /* Pixel Position */
                    //  ---------
                    //  |   |   |
                    //  ---------       center: centerPointRounded
                    //  |   | x |
                    //  ---------
                    /* Verify if not written yet */
                    if(!m_p_bPixelFilled[(iCenterPointXRounded)+((iCenterPointYRounded)*(*m_p_iImageWidth))])
                    {
                        /* Write pixel */
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded)*(*m_p_iImageWidth))*3] = m_ucRed[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded)*(*m_p_iImageWidth))*3+1] = m_ucGreen[ucSortedByDstFromScreen[i]];
                        m_p_ucImageData[(iCenterPointXRounded+(iCenterPointYRounded)*(*m_p_iImageWidth))*3+2] = m_ucBlue[ucSortedByDstFromScreen[i]];
                        m_p_bPixelFilled[(iCenterPointXRounded)+((iCenterPointYRounded)*(*m_p_iImageWidth))] = true;
                    }
                }
            }
        }
    }
}
