#include "gvtransform.h"

Transform::Transform()
{
    setAngles(0,0);
    setTranslation(0,0,0);
}

void Transform::setAngles(float dAngleX, float dAngleY, float dAngleZ)
{
    m_dAngleX = dAngleX;
    m_dAngleY = dAngleY;
    m_dAngleZ = dAngleZ;
}

void Transform::setTranslation(float x ,float y, float z)
{
    m_dX = x;
    m_dY = y;
    m_dZ = z;
}

void Transform::setUnrotatedCornersCorners(int iCenterPointX, int iCenterPointY, int iSideLenght)
{
    m_iCenterPointX = iCenterPointX;
    m_iCenterPointY = iCenterPointY;
    cout << "Center Point X : " << iCenterPointX << endl;
    cout << "Center Point Y : " << iCenterPointY << endl;

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

    /*for(int i = 0; i < 8; i++){
        cout << "Unrotated Corner " << i << " X : " << m_iUnrotatedCornerX[i] << endl;
        cout << "Unrotated Corner " << i << " Y : " << m_iUnrotatedCornerY[i] << endl;
        cout << "Unrotated Corner " << i << " Z : " << m_iUnrotatedCornerZ[i] << endl;
    }//*/
}

void Transform::computeTransform(float *dScreenTransformedCornerX,
                                 float *dScreenTransformedCornerY,
                                 float *dTransformedCornerZ)
{
    generateRotationMatrix();

    /* Compute Rotated Points */
    for(int i = 0; i < 8; i++)
    {
        dScreenTransformedCornerX[i] = m_dRotationMatrix[0][0]*(m_iUnrotatedCornerX[i] + m_dX) +
                                       m_dRotationMatrix[0][1]*(m_iUnrotatedCornerY[i] + m_dY) +
                                       m_dRotationMatrix[0][2]*(m_iUnrotatedCornerZ[i] + m_dZ +
                                                                DST_OCULUS_ORIGIN);

        dScreenTransformedCornerY[i] = m_dRotationMatrix[1][0]*(m_iUnrotatedCornerX[i] + m_dX) +
                                       m_dRotationMatrix[1][1]*(m_iUnrotatedCornerY[i] + m_dY) +
                                       m_dRotationMatrix[1][2]*(m_iUnrotatedCornerZ[i] + m_dZ +
                                                                DST_OCULUS_ORIGIN);

        dTransformedCornerZ[i] = m_dRotationMatrix[2][0]*(m_iUnrotatedCornerX[i] + m_dX) +
                                 m_dRotationMatrix[2][1]*(m_iUnrotatedCornerY[i] + m_dY) +
                                 m_dRotationMatrix[2][2]*(m_iUnrotatedCornerZ[i] + m_dZ +
                                                          DST_OCULUS_ORIGIN);
    }
    /*for(int i = 0; i < 8; i++){
        cout << "Rotated Corner " << i << " X : " << dScreenTransformedCornerX[i] << endl;
        cout << "Rotated Corner " << i << " Y : " << dScreenTransformedCornerY[i] << endl;
        cout << "Rotated Corner " << i << " Z : " << dTransformedCornerZ[i] << endl;
    }//   DEBUG */
}

void Transform::getImageCenterPoint(int* centerPointX, int* centerPointY)
{
    float x = m_dRotationMatrix[0][0]*(m_dX) +
            m_dRotationMatrix[0][1]*( m_dY) +
            m_dRotationMatrix[0][2]*( m_dZ) +
            m_iCenterPointX;

    float y = m_dRotationMatrix[1][0]*( m_dX) +
            m_dRotationMatrix[1][1]*(m_dY) +
            m_dRotationMatrix[1][2]*(m_dZ) +
            m_iCenterPointY;

    (*centerPointX) = (int)x;
    (*centerPointY) = (int)y;
}

void Transform::generateRotationMatrix()
{
    /* Precomputation */
    float cosAngleX = fastCos(m_dAngleX);
    float sinAngleX = fastSin(m_dAngleX);
    float cosAngleY = fastCos(m_dAngleY);
    float sinAngleY = fastSin(m_dAngleY);
    float cosAngleZ = fastCos(m_dAngleZ);
    float sinAngleZ = fastSin(m_dAngleZ);

    float cosXSinY = cosAngleX * sinAngleY;
    float sinXSinY = sinAngleX * sinAngleY;

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

//WARNING: NO PROTECTION: VALUE MUST BETWEEN -2PI and 2PI.
float Transform::fastSin(float x)
{
    //http://lab.polygonal.de/?p=205
    float sin = 0;
    if (x < -3.14159265){
        x += 6.28318531;
    }
    else if (x >  3.14159265){
        x -= 6.28318531;
    }

    //compute sine
    if (x < 0){
        sin = 1.27323954 * x + .405284735 * x * x;
    }
    else{
        sin = 1.27323954 * x - 0.405284735 * x * x;
    }

    return sin;
}

//WARNING: NO PROTECTION: VALUE MUST BETWEEN -2PI and 2PI.
float Transform::fastCos(float value)
{
    return fastSin(value + PI/2);
}
