#ifndef GVTRANSFORM_H
#define GVTRANSFORM_H

#include <math.h>


#include <iostream>
using namespace std;

#define PI  3.14159265359

#define DST_OCULUS_ORIGIN   500     //mm

/**************************************************************************
 * The purpose of this class is to abstract the the transformations maths.
 **************************************************************************/

class Transform
{
public:
    Transform();

    /* Setters */
    void setUnrotatedCornersCorners(int iCenterPointX, int iCenterPointY, int iSideLenght);

    void setAngles(float dAngleX, float dAngleY, float dAngleZ = 0);
    void setTranslation(float x ,float y, float z);

    /* Computing */
    void computeTransform(float* dScreenTransformedCornerX,
                          float* dScreenTransformedCornerY,
                          float* dTransformedCornerZ);
    void getImageCenterPoint(int* centerPointX, int* centerPointY);

    /* Matrix Op */
    void generateRotationMatrix();

    //WARNING SIN & COS: NO PROTECTION: VALUE MUST BETWEEN -2PI and 2PI.
    float fastSin(float value);
    float fastCos(float value);

private:
    /* Angles */
    float m_dAngleX;
    float m_dAngleY;
    float m_dAngleZ;

    /* Translation */
    float m_dX;
    float m_dY;
    float m_dZ;

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
    float m_dRotationMatrix[4][4];
};

#endif // GVTRANSFORM_H
