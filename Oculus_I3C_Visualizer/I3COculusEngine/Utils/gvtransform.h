#ifndef GVTRANSFORM_H
#define GVTRANSFORM_H

#include <math.h>


#include <iostream>
using namespace std;

#define PI  3.14159265359

#define DST_OCULUS_ORIGIN   500

/**************************************************************************
 * The purpose of this class is to abstract the the transformations maths.
 **************************************************************************/

class Transform
{
public:
    Transform();

    /* Setters */
    void setUnrotatedCornersCorners(int iCenterPointX, int iCenterPointY, int iSideLenght);
    void setFOV(float down, float up, float right, float left);

    void setAngles(double dAngleX, double dAngleY, double dAngleZ = 0);
    void setTranslation(double x ,double y, double z);

    /* Computing */
    void computeTransform(double* dScreenTransformedCornerX,
                          double* dScreenTransformedCornerY,
                          double* dTransformedCornerZ);
    void getImageCenterPoint(int* centerPointX, int* centerPointY);

    /* Matrix Op */
    void generateRotationMatrix();

private:
    /* Angles */
    double m_dAngleX;
    double m_dAngleY;
    double m_dAngleZ;

    /* Translation */
    double m_dX;
    double m_dY;
    double m_dZ;

    /* FOV */
    float m_fFOVUp;
    float m_fFOVDown;
    float m_fFOVIn;
    float m_fFOVOut;

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
