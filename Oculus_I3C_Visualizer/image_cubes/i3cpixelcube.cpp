/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CPixelCube.h
 * Creation :   May 14th 2015
 * Purpose  :   Represent a Cube of pixel
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class represent a cube storing up to 8 pixel values.
 * *********************************************************/

#include "i3cpixelcube.h"

I3CPixelCube::I3CPixelCube(cl_context *context,
                           cl_command_queue *commandQueue,
                           cl_kernel *drawPixelKernel,
                           cl_mem *pixelPosition,
                           cl_mem *pixelColor):I3CCube(context, commandQueue)
{
    m_ucRed = NULL;
    m_ucGreen = NULL;
    m_ucBlue = NULL;

    /*m_pixelPosition = pixelPosition;
    m_pixelColor = pixelColor;*/
    m_drawPixelKernel = drawPixelKernel;
}

I3CPixelCube::~I3CPixelCube()
{
    if(m_ucRed != NULL){
        delete[] m_ucRed;
    }
    if(m_ucGreen != NULL){
        delete[] m_ucGreen;
    }
    if(m_ucBlue != NULL){
        delete[] m_ucBlue;
    }
}

void I3CPixelCube::addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue)
{
    m_ucMap = ucMap;
    m_iHierarchyLevel = 0;
    int i_Counter = 0;

    //Create storage according to the map
    m_ucRed = new unsigned char[8];
    m_ucGreen = new unsigned char[8];
    m_ucBlue = new unsigned char[8];

    //Set the value for each specified pixel
    for(int i = 0; i < 8; i++)
    {
        if(m_ucMap & (0x01 << i)){
            m_ucRed[i] = (unsigned char)ucRed[i_Counter];
            m_ucGreen[i] = (unsigned char)ucGreen[i_Counter];
            m_ucBlue[i] = (unsigned char)ucBlue[i_Counter];
            i_Counter++;
        }
    }
}

void I3CPixelCube::render(cl_mem *corners)
{
    //To debug what has been done previously: draw black points on each corners
}

void I3CPixelCube::render(Coordinate corners[8])
{
    cl_float4 colorTmp = {0.0, 0.0, 0.0, 1.0};
    cl_int2 pos = {500, 500};

    /*clEnqueueWriteBuffer(*m_clCommandQueue, *m_pixelColor, CL_TRUE, 0, sizeof(colorTmp),
                         &colorTmp, 0, NULL, NULL);
    clEnqueueWriteBuffer(*m_clCommandQueue, *m_pixelPosition, CL_TRUE, 0, sizeof(pos),
                         &pos, 0, NULL, NULL);*/
    size_t workItems = 1;
    cl_int error = clEnqueueNDRangeKernel(*m_clCommandQueue, *m_drawPixelKernel, 1, NULL,
                           &workItems , NULL, 0, NULL, NULL);
}
