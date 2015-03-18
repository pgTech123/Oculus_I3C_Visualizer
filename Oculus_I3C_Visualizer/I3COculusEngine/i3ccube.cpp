#include "i3ccube.h"

I3CCube::I3CCube():GVIndexCube()
{
}

I3CCube::I3CCube(int* p_iImageWidth,
                 int* p_iImageHeight,
                 unsigned char* p_ucImageData,
                 bool* p_bPixelFilled):
       GVIndexCube(p_iImageWidth, p_iImageHeight, p_ucImageData, p_bPixelFilled)
{
}

void I3CCube::render(float iArrPosX[8],
                     float iArrPosY[8],
                     float iArrPosZ[8],
                     unsigned char ucSortedByDstFromScreen[8])
{
    // Render cubes accordingly to their level
    if(m_iHierarchyLevel > 0){
        renderReference(iArrPosX,
                    iArrPosY,
                    iArrPosZ,
                    ucSortedByDstFromScreen);
    }
    else{
        renderPixels(iArrPosX,
                    iArrPosY,
                    iArrPosZ,
                    ucSortedByDstFromScreen);
    }
}

void I3CCube::renderReference(float iArrPosX[8],
                              float iArrPosY[8],
                              float iArrPosZ[8],
                              unsigned char ucSortedByDstFromScreen[8])
{
    //Compute subcorners: if all z -, return
}

void I3CCube::renderPixels(float iArrPosX[8],
                           float iArrPosY[8],
                           float iArrPosZ[8],
                           unsigned char ucSortedByDstFromScreen[8])
{

}
