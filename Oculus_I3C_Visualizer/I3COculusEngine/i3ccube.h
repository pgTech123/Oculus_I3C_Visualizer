#ifndef I3CCUBE_H
#define I3CCUBE_H

//ERRORS
#define NO_ERR                  0
#define ERR_BAD_WIDTH_PTR       500
#define ERR_BAD_HEIGHT_PTR      501
#define ERR_NULL_IMG_PTR        502
#define ERR_NULL_FILLED_PX_PTR  503
#define ERR_NULL_SCREEN         504


#include <iostream>

//DEBUG
using namespace std;
//END DEBUG


class Coordinate
{
public:
    float x;
    float y;
    float z;

    void fromMidCoord(Coordinate coord1, Coordinate coord2);
};

typedef struct{
    float up;
    float down;
    float left;
    float right;
}FOV;

typedef struct{
    //Value between 0 and 1
    //Up/Down Ratio
    float up_downRatio;
    //Left/Right Ratio
    float left_rightRatio;

    int up;
    int down;
    int left;
    int right;

    int lenseCenterX;
    int lenseCenterY;

    float focalLength;
}RenderingScreen;

typedef struct{
    int width;
    int height;
    //Where X and Y are in the upper left corner
    int x;
    int y;
}BoundingRect;


/* *******************************************************************
 * Warning: I3CCube consider that the render function won't be called
 *          if there was an error of an sort setting ImageProperties.
 * *******************************************************************/

class I3CCube
{
public:
    I3CCube();
    I3CCube(int* p_iImageWidth,
            int* p_iImageHeight,
            unsigned char* p_ucImageData,
            unsigned char* p_ucPixelFilled,
            RenderingScreen* p_screen);
    virtual ~I3CCube();

    //Accessors
    int setImageProperty(int* p_iImageWidth,
                         int* p_iImageHeight,
                         unsigned char* p_ucImageData,
                         unsigned char* p_ucPixelFilled,
                         RenderingScreen* p_screen);
    int getHierarchyLevel();

    //Image
    void addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue);
    void addReferenceCube(unsigned char ucMap, I3CCube** p_ChildCubeRef);

    //Rendering
    void render(float iArrPosX[8],
                float iArrPosY[8],
                float iArrPosZ[8],
                unsigned char ucSortedByDstFromScreen[8]);

    void render(Coordinate iArrPos[8],
                unsigned char ucSortedByDstFromScreen[8]);

private:
    //Initialization
    void initializeCube();


    //Rendering
    void renderReference(Coordinate iArrPos[8],
                         unsigned char ucSortedByDstFromScreen[8]);

    void renderPixels(Coordinate iArrPos[8],
                      unsigned char ucSortedByDstFromScreen[8]);

    void computeSubcorners();

    void renderChildIfZPositive(unsigned char cubeId,
                                unsigned char ucSortedByDstFromScreen[8]);

    void tryToDrawPixel(int up, int down, int left, int right,
                        unsigned char cubeId);

    BoundingRect findBoundingRect(Coordinate corners[8]);

    bool childIsHidden(Coordinate childCorners[8]);

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
    int m_iTotalPixels;

    //Computing data
    Coordinate m_fArrSubcorners[27];

    //Screen
    RenderingScreen* m_pScreen;
};

#endif // I3CCUBE_H
