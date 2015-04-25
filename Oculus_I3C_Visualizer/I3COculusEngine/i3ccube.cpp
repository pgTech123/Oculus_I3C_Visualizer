#include "i3ccube.h"

void Coordinate::fromMidCoord(Coordinate coord1, Coordinate coord2)
{
    this->x = (coord1.x + coord2.x) / 2;
    this->y = (coord1.y + coord2.y) / 2;
    this->z = (coord1.z + coord2.z) / 2;
}

void Coordinate::fromMidCoordXYOnly(Coordinate coord1, Coordinate coord2)
{
    this->x = (coord1.x + coord2.x) / 2;
    this->y = (coord1.y + coord2.y) / 2;
}

I3CCube::I3CCube()
{
    initializeCube();
}

I3CCube::I3CCube(int* p_iImageWidth,
                 int* p_iImageHeight,
                 unsigned char* p_ucImageData,
                 unsigned char* p_ucPixelFilled,
                 RenderingScreen* p_screen)
{
    initializeCube();
    setImageProperty(p_iImageWidth, p_iImageHeight, p_ucImageData, p_ucPixelFilled, p_screen);
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
                              unsigned char* p_ucPixelFilled,
                              RenderingScreen* p_screen)
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
    m_iTotalPixels = (*m_piImageWidth) * (*m_piImageHeight);
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
    if(p_screen != NULL){
        m_pScreen = p_screen;
        //cout << m_pScreen << endl;
    }else{
        //cout << "ERROR p_Screen" << endl;
        return ERR_NULL_SCREEN;
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
    //Fill outter corners
    int outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    for(int i = 0; i < 8; i++){
        m_fArrSubcorners[outterCornersIndex[i]] = iArrPos[i];
    }
    computeSubcorners();

    for(int i = 0; i < 8; i ++){
        if((m_ucMap & (0x01 << ucSortedByDstFromScreen[i]))){
            renderChildIfZPositive(ucSortedByDstFromScreen[i],
                                   ucSortedByDstFromScreen);
        }
    }
}

void I3CCube::renderPixels(Coordinate iArrPos[8],
                           unsigned char ucSortedByDstFromScreen[8])
{
    //Here, m_fArrSubcorners is used to represent the screen coordinate
    //so the "z" coordinate is not used.
    //We compute the perspective: Focal Length * (X or Y) / Z
    int outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    for(int i = 0; i < 8; i++){
        m_fArrSubcorners[outterCornersIndex[i]].x = (iArrPos[i].x * m_pScreen->focalLength) / iArrPos[i].z;
        m_fArrSubcorners[outterCornersIndex[i]].y = (iArrPos[i].y * m_pScreen->focalLength) / iArrPos[i].z;
    }

    computeSubcornersXY_Only();

    //Draw
    for(int i = 0; i < 8; i ++){
        if((m_ucMap & (0x01 << ucSortedByDstFromScreen[i]))){
            tryToDrawPixel(m_pScreen->up, m_pScreen->down, m_pScreen->left, m_pScreen->right,
                           ucSortedByDstFromScreen[i]);
        }
    }
}

void I3CCube::computeSubcorners()
{
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

void I3CCube::computeSubcornersXY_Only()
{
    //Compute Mid
    int midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    int computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                 {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    for(int i = 0; i < 12; i++){
        m_fArrSubcorners[midCornersIndex[i]].fromMidCoordXYOnly(m_fArrSubcorners[computeMidWith[i][0]],
                                                                m_fArrSubcorners[computeMidWith[i][1]]);
    }

    //Compute mid face
    int midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    int computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};
    for(int i = 0; i < 6; i++){
        m_fArrSubcorners[midFacesIndex[i]].fromMidCoordXYOnly(m_fArrSubcorners[computeMidFaceWith[i][0]],
                                                              m_fArrSubcorners[computeMidFaceWith[i][1]]);
    }

    //Compute center
    int cubeCenter[1] = {13};
    int computeFrom[1][2] = {{10,16}};
    m_fArrSubcorners[cubeCenter[0]].fromMidCoordXYOnly(m_fArrSubcorners[computeFrom[0][0]],
                                                       m_fArrSubcorners[computeFrom[0][1]]);
}

void I3CCube::renderChildIfZPositive(unsigned char cubeId,
                                     unsigned char ucSortedByDstFromScreen[8])
{
    int childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};
    Coordinate childCorners[8];

    childCorners[0] = m_fArrSubcorners[childBaseCorner[cubeId]];
    childCorners[1] = m_fArrSubcorners[childBaseCorner[cubeId]+1];
    childCorners[2] = m_fArrSubcorners[childBaseCorner[cubeId]+10];
    childCorners[3] = m_fArrSubcorners[childBaseCorner[cubeId]+9];
    childCorners[4] = m_fArrSubcorners[childBaseCorner[cubeId]+3];
    childCorners[5] = m_fArrSubcorners[childBaseCorner[cubeId]+4];
    childCorners[6] = m_fArrSubcorners[childBaseCorner[cubeId]+13];
    childCorners[7] = m_fArrSubcorners[childBaseCorner[cubeId]+12];

    //If all Z are negative, stop rendering: won't be seen
    for(int i = 0; i < 8; i++){
        if(childCorners[i].z > 0){
            break;
        }
        return; //All negative: stop rendering this branch
    }

    //This implementation suppose that most pixels are hidden on one frame
    if(!childIsHidden(childCorners)){
        m_pArrChildCubes[cubeId]->render(childCorners, ucSortedByDstFromScreen);
    }
}

void I3CCube::tryToDrawPixel(int up, int down, int left, int right,
                             unsigned char cubeId)
{
    int childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};
    Coordinate pixelCorners[8];

    //TODO: MODIFY FOR A LOOP
    pixelCorners[0] = m_fArrSubcorners[childBaseCorner[cubeId]];
    pixelCorners[1] = m_fArrSubcorners[childBaseCorner[cubeId]+1];
    pixelCorners[2] = m_fArrSubcorners[childBaseCorner[cubeId]+10];
    pixelCorners[3] = m_fArrSubcorners[childBaseCorner[cubeId]+9];
    pixelCorners[4] = m_fArrSubcorners[childBaseCorner[cubeId]+3];
    pixelCorners[5] = m_fArrSubcorners[childBaseCorner[cubeId]+4];
    pixelCorners[6] = m_fArrSubcorners[childBaseCorner[cubeId]+13];
    pixelCorners[7] = m_fArrSubcorners[childBaseCorner[cubeId]+12];

    BoundingRect bound = findBoundingRect(pixelCorners);

    //Check if seen
    int boundDown = bound.y - bound.height;
    int boundRight = bound.x + bound.width;
    if(bound.y <= down || boundDown > up){
        return;
    }

    if(boundRight < left || bound.x >= right){
        return;
    }

    //Crop only what is seen
    if(bound.y > up){
        bound.y = up;
    }
    if(boundDown < down){
        bound.height = bound.y - down;
    }
    if(bound.x < left){
        bound.x = left;
    }
    if(boundRight > right){
        bound.width = right - bound.x;
    }

    // As we don't expect the pixels to be seen (from too close),
    // it seems appropriate to approximate a 3D pixel as a square
    // with no orientation relative to the screen.
    int fillingOrigin = bound.x - left + ((bound.y - down) * (*m_piImageWidth));

    int imageIndex = fillingOrigin;
    for(int iY = 0; iY < bound.height; iY++){
        for(int iX = 0; iX < bound.width; iX++){
            //Safety first :P
            /*if(imageIndex >= m_iTotalPixels || imageIndex < 0){
                return;
            }*/
            if(m_pucPixelFilled[imageIndex] == 0){
                //Write Pixel
                m_pucImageData[imageIndex*3] = m_ucRed[cubeId];
                m_pucImageData[imageIndex*3 + 1] = m_ucGreen[cubeId];
                m_pucImageData[imageIndex*3 + 2] = m_ucBlue[cubeId];
                //Alpha
                m_pucPixelFilled[imageIndex] = 255;
            }
            imageIndex ++;
        }
        imageIndex = fillingOrigin + (*m_piImageWidth);
    }

    //FUTUR: implement linear interpolation with alpha...
}

//Make this function procedural
BoundingRect I3CCube::findBoundingRect(Coordinate corners[8])
{
    int lowerValueX = 1000000;
    int lowerValueY = 1000000;
    int higherValueX = -1000000;
    int higherValueY = -1000000;
    BoundingRect bounding;

    for(int i = 0; i < 8; i++){
        if(corners[i].x < lowerValueX){
            lowerValueX = (int)corners[i].x;
        }
        if(corners[i].y < lowerValueY){
            lowerValueY = (int)corners[i].y;
        }
        if(corners[i].x > higherValueX){
            higherValueX = (int)corners[i].x;
        }
        if(corners[i].y > higherValueY){
            higherValueY = (int)corners[i].y;
        }
    }

    bounding.x = (int)corners[0].x;
    bounding.y = (int)corners[0].y;
    bounding.width = higherValueX - lowerValueX;
    bounding.height = higherValueY - lowerValueY;

    return bounding;
}

bool I3CCube::childIsHidden(Coordinate childCorners[8])
{
    //MAJOR BUG: Cannot access m_pScreen ...
    //cout << m_pScreen << endl;
    /*Coordinate cornersProjected[8];
    for(int i = 0; i < 8; i++){
        //TODO: 600 should be |m_pScreen->focalLength|
        cornersProjected[i].x = ((float)childCorners[i].x * m_pScreen->focalLength) / (float)childCorners[i].z;
        cornersProjected[i].y = ((float)childCorners[i].y * m_pScreen->focalLength) / (float)childCorners[i].z;
    }

    //cout << cornersProjected[0].x << endl;

    BoundingRect bound = findBoundingRect(cornersProjected);
    //Check if seen
    int boundDown = bound.y - bound.height;
    int boundRight = bound.x + bound.width;
    if(bound.y <= m_pScreen->down || boundDown > m_pScreen->up){
        return true;
    }

    if(boundRight < m_pScreen->left || bound.x >= m_pScreen->right){
        return true;
    }

    //Crop only what is seen
    if(bound.y > m_pScreen->up){
        bound.y = m_pScreen->up;
    }
    if(boundDown < m_pScreen->down){
        bound.height = bound.y - m_pScreen->down;
    }
    if(bound.x < m_pScreen->left){
        bound.x = m_pScreen->left;
    }
    if(boundRight > m_pScreen->right){
        bound.width = m_pScreen->right - bound.x;
    }

    // As we don't expect the pixels to be seen (from too close),
    // it seems appropriate to approximate a 3D pixel as a square
    // with no orientation relative to the screen.
    int fillingOrigin = bound.x - m_pScreen->left + ((bound.y - m_pScreen->down) * (*m_piImageWidth));

    int imageIndex = fillingOrigin;
    for(int iY = 0; iY < bound.height; iY++){
        for(int iX = 0; iX < bound.width; iX++){
            if(m_pucPixelFilled[imageIndex] != 0){
                return false;
            }
            imageIndex ++;
        }
        imageIndex = fillingOrigin + (*m_piImageWidth);
    }
    return true;*/
    return false;
}
