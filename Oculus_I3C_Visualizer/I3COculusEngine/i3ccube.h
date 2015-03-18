#ifndef I3CCUBE_H
#define I3CCUBE_H

#include "i3cindexcube.h"


//THE OBJECTIVE INHERITANCE IS TO REMOVE GVIndexCube slowly
class I3CCube: public GVIndexCube
{
public:
    I3CCube();
    I3CCube(int* p_iImageWidth,
            int* p_iImageHeight,
            unsigned char* p_ucImageData,
            bool* p_bPixelFilled);

    void render(float iArrPosX[8],
                float iArrPosY[8],
                float iArrPosZ[8],
                unsigned char ucSortedByDstFromScreen[8]);

private:
    void renderReference(float iArrPosX[8],
                         float iArrPosY[8],
                         float iArrPosZ[8],
                         unsigned char ucSortedByDstFromScreen[8]);

    void renderPixels(float iArrPosX[8],
                      float iArrPosY[8],
                      float iArrPosZ[8],
                      unsigned char ucSortedByDstFromScreen[8]);


private:
    int m_iHierarchyLevel;
};

#endif // I3CCUBE_H
