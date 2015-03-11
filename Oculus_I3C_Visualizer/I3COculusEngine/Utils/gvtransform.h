#ifndef GVTRANSFORM_H
#define GVTRANSFORM_H

#include <math.h>


#include <iostream>
using namespace std;

#define PI  3.14159265359

/**************************************************************************
 * The purpose of this class is to abstract the the transformations maths.
 **************************************************************************/

class GVTransform
{
public:
    GVTransform();

    /* Setters */
    void setUnrotatedCornersCorners(int iCenterPointX, int iCenterPointY, int iSideLenght);
    void setAngles(double dAngleX, double dAngleY, double dAngleZ = 0);

    /* Computing */
    void computeRotation(double* dScreenRotatedCornerX, double* dScreenRotatedCornerY, double* dRotatedCornerZ);

    /* Matrix Op */
    void generateRotationMatrix();

private:
    /* Angles */
    double m_dAngleX;
    double m_dAngleY;
    double m_dAngleZ;

    /* Unrotated Corners */
    int m_iUnrotatedCornerX[8];
    int m_iUnrotatedCornerY[8];
    int m_iUnrotatedCornerZ[8];

    /* Image Center Point */
    int m_iCenterPointX;
    int m_iCenterPointY;

    /* Rotation Matrix */
    double m_dRotationMatrix[4][4];
};

#endif // GVTRANSFORM_H
