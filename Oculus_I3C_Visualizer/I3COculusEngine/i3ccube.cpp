#include "i3ccube.h"

void Coordinate::fromMidCoord(Coordinate coord1, Coordinate coord2)
{
    this->x = (coord1.x + coord2.x) / 2;
    this->y = (coord1.y + coord2.y) / 2;
    this->z = (coord1.z + coord2.z) / 2;
}

I3CCube::I3CCube()
{
    initializeCube();
}

I3CCube::I3CCube(int* p_iImageWidth,
                 int* p_iImageHeight,
                 unsigned char* p_ucImageData,
                 unsigned char* p_ucPixelFilled)
{
    initializeCube();
    setImageProperty(p_iImageWidth, p_iImageHeight, p_ucImageData, p_ucPixelFilled);
}

I3CCube::~I3CCube()
{
    if(m_ucRed != NULL){
        delete[] m_ucRed;
    }
    if(m_ucGreen != NULL){
        delete[] m_ucGreen;
    }
    if(m_ucBlue != NULL){
        delete[] m_ucBlue;
    }
    if(m_pArrChildCubes != NULL){
        delete[] m_pArrChildCubes;
    }
}

int I3CCube::setImageProperty(int* p_iImageWidth,
                               int* p_iImageHeight,
                               unsigned char* p_ucImageData,
                               unsigned char* p_ucPixelFilled)
{
    if(*p_iImageWidth > 0){
        m_piImageWidth = p_iImageWidth;
    }else{
        return ERR_BAD_WIDTH_PTR;
    }
    if(*p_iImageHeight > 0){
        m_piImageHeight = p_iImageHeight;
    }else{
        return ERR_BAD_HEIGHT_PTR;
    }
    if(p_ucImageData != NULL){
        m_pucImageData = p_ucImageData;
    }else{
        return ERR_NULL_IMG_PTR;
    }
    if(p_ucPixelFilled != NULL){
        m_pucPixelFilled = p_ucPixelFilled;
    }else{
        return ERR_NULL_FILLED_PX_PTR;
    }
    return NO_ERR;
}

int I3CCube::getHierarchyLevel()
{
    return m_iHierarchyLevel;
}

void I3CCube::addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue)
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

void I3CCube::addReferenceCube(unsigned char ucMap, I3CCube** p_ChildCubeRef)
{
    /* Set hierarchy level */
    m_iHierarchyLevel = p_ChildCubeRef[0]->getHierarchyLevel() + 1;

    /* Create storage according to the map */
    m_ucMap = ucMap;
    m_pArrChildCubes = new I3CCube*[8];

    /* Set the reference for each specified child cube */
    int i_Counter = 0;
    for(int i = 0; i < 8; i++)
    {
        if(m_ucMap & (0x01 << i)){
            m_pArrChildCubes[i] = (I3CCube*)p_ChildCubeRef[i_Counter];
            i_Counter++;
        }
    }
}

void I3CCube::render(float iArrPosX[8],
                     float iArrPosY[8],
                     float iArrPosZ[8],
                     unsigned char ucSortedByDstFromScreen[8])
{
    //Convert Pos to coodinates
    Coordinate coord[8];
    for(int i = 0; i < 8; i++){
        coord[i].x = iArrPosX[i];
        coord[i].y = iArrPosY[i];
        coord[i].z = iArrPosZ[i];
    }
    render(coord, ucSortedByDstFromScreen);
}

void I3CCube::render(Coordinate iArrPos[8],
                     unsigned char ucSortedByDstFromScreen[8])
{
    // Render cubes accordingly to their level
    if(m_iHierarchyLevel > 0){
        renderReference(iArrPos, ucSortedByDstFromScreen);
    }
    else{
        renderPixels(iArrPos, ucSortedByDstFromScreen);
    }
}

void I3CCube::initializeCube()
{
    m_piImageWidth = NULL;
    m_piImageHeight = NULL;
    m_pucImageData = NULL;
    m_pucPixelFilled = NULL;
    m_iHierarchyLevel = -1;

    m_ucRed = NULL;
    m_ucGreen = NULL;
    m_ucBlue = NULL;
    m_pArrChildCubes = NULL;
}


void I3CCube::renderReference(Coordinate iArrPos[8],
                              unsigned char ucSortedByDstFromScreen[8])
{
    computeSubcorners(iArrPos);

    for(int i = 0; i < 8; i ++){
        if((m_ucMap & (0x01 << ucSortedByDstFromScreen[i]))){
            renderChildIfZPositive(ucSortedByDstFromScreen[i], ucSortedByDstFromScreen);
        }
    }
}

void I3CCube::renderPixels(Coordinate iArrPos[8],
                           unsigned char ucSortedByDstFromScreen[8])
{
    //Compute(x/z and y/z) cube2pixel (pixel size)
    //Draw and adjust alpha if interpolation
}

void I3CCube::computeSubcorners(Coordinate iArrPos[8])
{
    //Fill outter corners
    int outterCornersIndex[8] = {0, 2, 8, 6, 18, 20, 24, 26};
    for(int i = 0; i < 8; i++){
        m_fArrSubcorners[outterCornersIndex[i]] = iArrPos[i];
    }

    //Compute Mid
    int midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    int computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                 {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    for(int i = 0; i < 12; i++){
        m_fArrSubcorners[midCornersIndex[i]].fromMidCoord(m_fArrSubcorners[computeMidWith[i][0]],
                                                          m_fArrSubcorners[computeMidWith[i][1]]);
    }

    //Compute mid face
    int midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    int computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};
    for(int i = 0; i < 6; i++){
        m_fArrSubcorners[midFacesIndex[i]].fromMidCoord(m_fArrSubcorners[computeMidFaceWith[i][0]],
                                                        m_fArrSubcorners[computeMidFaceWith[i][1]]);
    }

    //Compute center
    int cubeCenter[1] = {13};
    int computeFrom[1][2] = {{10,16}};
    m_fArrSubcorners[cubeCenter[0]].fromMidCoord(m_fArrSubcorners[computeFrom[0][0]],
                                                 m_fArrSubcorners[computeFrom[0][1]]);
}

void I3CCube::renderChildIfZPositive(unsigned char cubeId,
                                     unsigned char ucSortedByDstFromScreen[8])
{
    int childBaseCorner[8] = {0, 1, 3, 4, 9, 10, 12, 13};
    Coordinate childCorners[8];

    childCorners[0] = m_fArrSubcorners[childBaseCorner[cubeId]];
    childCorners[1] = m_fArrSubcorners[childBaseCorner[cubeId]+1];
    childCorners[2] = m_fArrSubcorners[childBaseCorner[cubeId]+3];
    childCorners[3] = m_fArrSubcorners[childBaseCorner[cubeId]+4];
    childCorners[4] = m_fArrSubcorners[childBaseCorner[cubeId]+9];
    childCorners[5] = m_fArrSubcorners[childBaseCorner[cubeId]+10];
    childCorners[6] = m_fArrSubcorners[childBaseCorner[cubeId]+12];
    childCorners[7] = m_fArrSubcorners[childBaseCorner[cubeId]+13];

    //If all Z are negative, stop rendering: won't be seen
    for(int i = 0; i < 8; i++){
        if(childCorners[i] > 0){
            break;
        }
        return; //All negative: stop rendering this branch
    }

    //TODO: Check zone covered by pixels already rendered: see if could improve performance???

    m_pArrChildCubes[ucSortedByDstFromScreen[cubeId]]->render(childCorners,
                                                              ucSortedByDstFromScreen);
}
