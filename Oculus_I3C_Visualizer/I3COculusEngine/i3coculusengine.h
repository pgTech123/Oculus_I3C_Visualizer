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

#include "Utils/gvtransform.h"

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


class I3COculusEngine
{
public:
    I3COculusEngine();
    ~I3COculusEngine();
    int openI3CFile(const char* filename);
    void setImageSize(int width, int height);


    void setRotation(double yaw, double pitch, double roll);
    void setPosition(double x, double y, double z);

    void generateImage();

    unsigned char* getData();

private:
    unsigned char* m_ucData;

    Transform m_Transform;
};

#endif // I3COCULUSENGINE_H
