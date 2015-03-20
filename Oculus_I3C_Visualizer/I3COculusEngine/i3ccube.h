#ifndef I3CCUBE_H
#define I3CCUBE_H

//ERRORS
#define NO_ERR                  0
#define ERR_BAD_WIDTH_PTR       500
#define ERR_BAD_HEIGHT_PTR      501
#define ERR_NULL_IMG_PTR        502
#define ERR_NULL_FILLED_PX_PTR  503


#include <iostream>


class Coordinate
{
public:
    float x;
    float y;
    float z;

    void fromMidCoord(Coordinate coord1, Coordinate coord2);
};


class I3CCube
{
public:
    I3CCube();
    I3CCube(int* p_iImageWidth,
            int* p_iImageHeight,
            unsigned char* p_ucImageData,
            unsigned char* p_ucPixelFilled);
    virtual ~I3CCube();

    //Accessors
    int setImageProperty(int* p_iImageWidth,
                          int* p_iImageHeight,
                          unsigned char* p_ucImageData,
                          unsigned char* p_ucPixelFilled);
    int getHierarchyLevel();

    //Image
    void addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue);
    void addReferenceCube(unsigned char ucMap, I3CCube** p_ChildCubeRef);

    //Rendering
    void render(float iArrPosX[8],
                float iArrPosY[8],
                float iArrPosZ[8],
                unsigned char ucSortedByDstFromScreen[8]);

private:
    //Initialization
    void initializeCube();


    //Rendering
    void renderReference(float iArrPosX[8],
                         float iArrPosY[8],
                         float iArrPosZ[8],
                         unsigned char ucSortedByDstFromScreen[8]);

    void renderPixels(float iArrPosX[8],
                      float iArrPosY[8],
                      float iArrPosZ[8],
                      unsigned char ucSortedByDstFromScreen[8]);

    void computeSubcorners(float iArrPosX[8],
                           float iArrPosY[8],
                           float iArrPosZ[8]);

private:
    //Hierarchy level
    int m_iHierarchyLevel;
    unsigned char m_ucMap;

    //Pixel level cube
    unsigned char* m_ucRed;
    unsigned char* m_ucGreen;
    unsigned char* m_ucBlue;

    //Reference cube
    I3CCube** m_pArrChildCubes;

    //Image Reference
    int* m_piImageWidth;
    int* m_piImageHeight;
    unsigned char* m_pucImageData;
    unsigned char* m_pucPixelFilled;

    //Computing data
    Coordinate m_fArrSubcorners[27];
};

#endif // I3CCUBE_H
