#include "gvtransform.h"

GVTransform::GVTransform()
{
    setAngles(0,0);
}

void GVTransform::setAngles(double dAngleX, double dAngleY, double dAngleZ)
{
    m_dAngleX = dAngleX;
    m_dAngleY = dAngleY;
    m_dAngleZ = dAngleZ;
}

void GVTransform::setUnrotatedCornersCorners(int iCenterPointX, int iCenterPointY, int iSideLenght)
{
    m_iCenterPointX = iCenterPointX;
    m_iCenterPointY = iCenterPointY;

    m_iUnrotatedCornerX[0] = -iSideLenght/2;
    m_iUnrotatedCornerY[0] = iSideLenght/2;
    m_iUnrotatedCornerZ[0] = iSideLenght/2;

    m_iUnrotatedCornerX[1] = iSideLenght/2;
    m_iUnrotatedCornerY[1] = iSideLenght/2;
    m_iUnrotatedCornerZ[1] = iSideLenght/2;

    m_iUnrotatedCornerX[2] = iSideLenght/2;
    m_iUnrotatedCornerY[2] = iSideLenght/2;
    m_iUnrotatedCornerZ[2] = -1*(iSideLenght/2);

    m_iUnrotatedCornerX[3] = -iSideLenght/2;
    m_iUnrotatedCornerY[3] = iSideLenght/2;
    m_iUnrotatedCornerZ[3] = -1*(iSideLenght/2);

    m_iUnrotatedCornerX[4] = -iSideLenght/2;
    m_iUnrotatedCornerY[4] = -iSideLenght/2;
    m_iUnrotatedCornerZ[4] = (iSideLenght/2);

    m_iUnrotatedCornerX[5] = iSideLenght/2;
    m_iUnrotatedCornerY[5] = -iSideLenght/2;
    m_iUnrotatedCornerZ[5] = (iSideLenght/2);

    m_iUnrotatedCornerX[6] = iSideLenght/2;
    m_iUnrotatedCornerY[6] = -iSideLenght/2;
    m_iUnrotatedCornerZ[6] = -1*(iSideLenght/2);

    m_iUnrotatedCornerX[7] = -iSideLenght/2;
    m_iUnrotatedCornerY[7] = -iSideLenght/2;
    m_iUnrotatedCornerZ[7] = -1*(iSideLenght/2);
}

void GVTransform::computeRotation(double *dScreenRotatedCornerX, double *dScreenRotatedCornerY, double *dRotatedCornerZ)
{
    generateRotationMatrix();

    /* Compute Rotated Points */
    for(int i = 0; i < 8; i++)
    {
        dScreenRotatedCornerX[i] = m_dRotationMatrix[0][0]*m_iUnrotatedCornerX[i] +
                                   m_dRotationMatrix[0][1]*m_iUnrotatedCornerY[i] +
                                   m_dRotationMatrix[0][2]*m_iUnrotatedCornerZ[i] + m_iCenterPointX;

        dScreenRotatedCornerY[i] = m_dRotationMatrix[1][0]*m_iUnrotatedCornerX[i] +
                                   m_dRotationMatrix[1][1]*m_iUnrotatedCornerY[i] +
                                   m_dRotationMatrix[1][2]*m_iUnrotatedCornerZ[i] + m_iCenterPointY;

        dRotatedCornerZ[i] = m_dRotationMatrix[2][0]*m_iUnrotatedCornerX[i] +
                                   m_dRotationMatrix[2][1]*m_iUnrotatedCornerY[i] +
                                   m_dRotationMatrix[2][2]*m_iUnrotatedCornerZ[i];
    }
}

void GVTransform::generateRotationMatrix()
{
    /* Precomputation */
    double cosAngleX = cos(m_dAngleX);
    double sinAngleX = sin(m_dAngleX);
    double cosAngleY = cos(m_dAngleY);
    double sinAngleY = sin(m_dAngleY);
    double cosAngleZ = cos(m_dAngleZ);
    double sinAngleZ = sin(m_dAngleZ);

    double cosXSinY = cosAngleX * sinAngleY;
    double sinXSinY = sinAngleX * sinAngleY;

    /* Matrix Assignation */
    m_dRotationMatrix[0][0] = cosAngleY * cosAngleZ;
    m_dRotationMatrix[0][1] = -cosAngleY * sinAngleZ;
    m_dRotationMatrix[0][2] = sinAngleY;
    m_dRotationMatrix[0][3] = 0;

    m_dRotationMatrix[1][0] = sinXSinY * cosAngleZ + cosAngleX*sinAngleZ;
    m_dRotationMatrix[1][1] = - sinXSinY * sinAngleZ + cosAngleX * cosAngleZ;
    m_dRotationMatrix[1][2] = - sinAngleX * cosAngleY;
    m_dRotationMatrix[1][3] = 0;

    m_dRotationMatrix[2][0] = - cosXSinY * cosAngleZ + sinAngleX * sinAngleZ;
    m_dRotationMatrix[2][1] = cosXSinY * sinAngleZ + sinAngleX * cosAngleZ;
    m_dRotationMatrix[2][2] = cosAngleX * cosAngleY;
    m_dRotationMatrix[2][3] = 0;

    m_dRotationMatrix[3][0] = 0;
    m_dRotationMatrix[3][1] = 0;
    m_dRotationMatrix[3][2] = 0;
    m_dRotationMatrix[3][3] = 1;

}
