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
    void ApplyRotation_and_Render(double iArrPosXRotation[8],
                                    double iArrPosYRotation[8],
                                    unsigned char ucSortedByDstFromScreen[8],
                                    double dCenterPointX,
                                    double dCenterPointY);

private:
    void initializeCube();
    void renderReference(double iArrPosXRotation[8],
                        double iArrPosYRotation[8],
                        unsigned char ucSortedByDstFromScreen[8],
                        double dCenterPointX,
                        double dCenterPointY);
    void renderPixel(double dArrPosXRotation[8],
                        double dArrPosYRotation[8],
                        unsigned char ucSortedByDstFromScreen[8],
                        double dCenterPointX,
                        double dCenterPointY);
    void computeMidArr(double* dArrPosXRotation,
                       double* dArrPosYRotation,
                       double* dMidArrX,
                       double* dMidArrY);
    void computeMidFace(double* dMidArrX,
                        double* dMidArrY,
                        double* dMidFaceXArr,
                        double* dMidFaceYArr);
    void computeChildCorners(double* dArrPosXRotation,
                        double* dArrPosYRotation,
                        unsigned char ucMapIndex,
                        double dCenterPointX,
                        double dCenterPointY,
                        double* dMidArrX,
                        double* dMidArrY,
                        double* dMidFaceXArr,
                        double* dMidFaceYArr);
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
    double m_dChildComputedCornersX[8];
    double m_dChildComputedCornersY[8];
    double m_dChildCenterPointX;
    double m_dChildCenterPointY;
};

#endif // GVINDEXCUBE_H
