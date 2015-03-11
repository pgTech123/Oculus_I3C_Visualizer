/*************************************************************************
 * Project   :      Oculus_I3C_Visualizer
 * Filename  :      i3coculusengine.cpp
 * Author    :      Pascal Gendron
 * Version   :      0.1
 *
 * Copyright :      GNU GENERAL PUBLIC LICENSE
 * ************************************************************************/

#include "i3coculusengine.h"

I3COculusEngine::I3COculusEngine()
{
    m_ucData = NULL;
}

I3COculusEngine::~I3COculusEngine()
{
    if(m_data != NULL){
        delete[] m_ucData;
    }
}

int I3COculusEngine::openI3CFile(const char* filename)
{
    //TODO: Open file
    //TODO: setUnrotatedCorners
}

void I3COculusEngine::setImageSize(int width, int height)
{
    m_ucData = new unsigned char[width * height];
}

void I3COculusEngine::setRotation(double yaw, double pitch, double roll)
{
    m_Transform.setAngles(yaw, pitch, roll);
}

void I3COculusEngine::setPosition(double x, double y, double z)
{
    m_Transform.setTranslation(x, y, z);
}

void I3COculusEngine::generateImage()
{
    //Apply transform
    //m_Transform.computeTransform(OUTPUT);

    //Render considering distortion
}

unsigned char* I3COculusEngine::getData()
{
    return m_ucData;
}
