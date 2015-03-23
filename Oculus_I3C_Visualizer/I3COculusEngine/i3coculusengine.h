/*************************************************************************
 * Project   :      Oculus_I3C_Visualizer
 * Filename  :      i3coculusengine.h
 * Author    :      Pascal Gendron
 * Version   :      0.1
 *
 * Copyright :      GNU GENERAL PUBLIC LICENSE
 * ************************************************************************/


#ifndef I3COCULUSENGINE_H
#define I3COCULUSENGINE_H

#include <fstream>
#include "i3ccube.h"
#include "Utils/gvtransform.h"
#include "Utils/gvbinaryfunctions.h"


/* Error Definition */
#define NO_ERRORS               0
#define UNABLE_TO_OPEN_FILE     101
#define INVALID_CUBE_SIZE       102
#define SIZE_NOT_BASE_2         103
#define FILE_CORRUPTED          104


/* **********************************************************************
 * The purpose of this class is to give an easy access
 * to manipulate I3C file and to use the optimized render
 * for the oculus.
 *
 * Typical call sequence:
 * -OpenI3CFile()
 * -setImageSize()  <- Must be call before generateImage() and getData()
 * -----LOOP-----
 *  -setRotation()
 *  -setPosition()
 *  -generateImage()
 *  -getData()
 * ---END LOOP---
 * **********************************************************************/


/* ********************
 * TODO:
 * -Add pixel density
 *
 * ********************/

class I3COculusEngine: public I3CCube
{
public:
    I3COculusEngine();
    virtual ~I3COculusEngine();
    int openI3CFile(const char* filename);
    bool setImageSize(int width, int height);
    void setFOV(float down, float up, float right, float left);

    void setRotation(double yaw, double pitch, double roll);
    void setPosition(double x, double y, double z);

    void generateImage();

    unsigned char* getData();

private:
    //File Functions
    int readImageFile(fstream *file);
    int verifyAndAssignSideLength(int iSideLength);
    void readNumOfMaps(fstream *file);
    void setImageCenterPoint();
    void setNumberOfLevels();

    int readCubes(fstream *file);
    int readPixelCubes(fstream *file);
    int readIndexCubes(fstream *file);
    int readMap(fstream *file, unsigned char* ucMap, int* iNumOfPix);

    void clearImageInMemory();

private:
    int m_iSideLength;
    int m_iNumberOfLevels;
    int* m_iArrCubeAtLevel;
    int m_iTotalNumberOfCubes;
    I3CCube** m_pGVImageArray;

    int m_width;
    int m_height;
    unsigned char* m_pucData;
    unsigned char* m_pucPixelsFilled;

    int m_iCenterPointX;
    int m_iCenterPointY;
    Transform m_Transform;

    float m_dScreenTransformedCornerX[8];
    float m_dScreenTransformedCornerY[8];
    float m_dDstFromScreenTransformed[8];
    unsigned char m_dCornerSortedByDst[8];
};

#endif // I3COCULUSENGINE_H
