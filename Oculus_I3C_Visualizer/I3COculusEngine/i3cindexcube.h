/*************************************************************************
 * Project   :      Oculus_I3C_Visualizer
 * Filename  :      i3cindexcube.h
 * Author    :      Pascal Gendron
 * Version   :      0.1
 *
 * Copyright :      GNU GENERAL PUBLIC LICENSE
 * ************************************************************************/

#ifndef GVINDEXCUBE_H
#define GVINDEXCUBE_H

#include <iostream>
#include <math.h>
#include "Utils/gvbinaryfunctions.h"

using namespace std;

/*************************************************************************
 * I3CIndexCube Class:
 *
 * The purpose of this class is to represent a group of 8 storage space.
 * These spaces can be filled by pixels values or by references to another
 * GVIndexCube(child). This is the smallest building bloc of 3D images.
 *************************************************************************/

class GVIndexCube
{
public:
    GVIndexCube();
    GVIndexCube(int* p_iImageWidth,
                int* p_iImageHeight,
                unsigned char* p_ucImageData,
                bool* p_bPixelFilled);
    ~GVIndexCube();

    /* Setters */
    bool setImageProperty(int* p_iImageWidth,
                          int* p_iImageHeight,
                          unsigned char* p_ucImageData,
                          bool* p_bPixelFilled);

    /* Getters */
    int getHierarchyLevel();

    /* Add pixel or reference */
    void addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue);
    void addReferenceCube(unsigned char ucMap, GVIndexCube** p_ChildCubeRef);

    /* Transform and render */
    void ApplyRotation_and_Render(float iArrPosXRotation[8],
                                    float iArrPosYRotation[8],
                                    unsigned char ucSortedByDstFromScreen[8],
                                    float dCenterPointX,
                                    float dCenterPointY);

private:
    void initializeCube();
    void renderReference(float iArrPosXRotation[8],
                        float iArrPosYRotation[8],
                        unsigned char ucSortedByDstFromScreen[8],
                        float dCenterPointX,
                        float dCenterPointY);
    void renderPixel(float dArrPosXRotation[8],
                        float dArrPosYRotation[8],
                        unsigned char ucSortedByDstFromScreen[8],
                        float dCenterPointX,
                        float dCenterPointY);
    void computeMidArr(float* dArrPosXRotation,
                       float* dArrPosYRotation,
                       float* dMidArrX,
                       float* dMidArrY);
    void computeMidFace(float* dMidArrX,
                        float* dMidArrY,
                        float* dMidFaceXArr,
                        float* dMidFaceYArr);
    void computeChildCorners(float* dArrPosXRotation,
                        float* dArrPosYRotation,
                        unsigned char ucMapIndex,
                        float dCenterPointX,
                        float dCenterPointY,
                        float* dMidArrX,
                        float* dMidArrY,
                        float* dMidFaceXArr,
                        float* dMidFaceYArr);
    void findCenterPoint();
    bool isChildFullyHidden();

protected:
    /********************************************************************
     * Global Image Data & References
     * (Initialized once for an image and pointer set in all child cubes)
     ********************************************************************/
    int* m_p_iImageWidth;
    int* m_p_iImageHeight;
    unsigned char* m_p_ucImageData;
    bool* m_p_bPixelFilled;

private:
    /* Cube info */
    int m_iHierarchyLevel;
    unsigned char m_ucMap;

    /* Pixel level cube */
    unsigned char* m_ucRed;
    unsigned char* m_ucGreen;
    unsigned char* m_ucBlue;

    /* Reference cube */
    GVIndexCube** m_p_GVIndexCubeArray;

    /* Reference cube rendering */
    float m_dChildComputedCornersX[8];
    float m_dChildComputedCornersY[8];
    float m_dChildCenterPointX;
    float m_dChildCenterPointY;
};

#endif // GVINDEXCUBE_H
