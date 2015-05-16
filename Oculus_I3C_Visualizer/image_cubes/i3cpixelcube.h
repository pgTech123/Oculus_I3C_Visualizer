/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CPixelCube.cpp
 * Creation :   May 14th 2015
 * Purpose  :   Represent a Cube of pixel
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class represent a cube storing up to 8 pixel values.
 * *********************************************************/

#ifndef I3CPIXELCUBE_H
#define I3CPIXELCUBE_H

#include <iostream>
#include <CL/cl.h>
#include "i3ccube.h"

class I3CPixelCube : public I3CCube
{
public:
    I3CPixelCube(cl_context *context,
                 cl_command_queue *commandQueue,
                 cl_mem *cubeDstSorted);
    ~I3CPixelCube();

    void addPixelsCube(unsigned char ucMap, int* ucRed, int* ucGreen, int* ucBlue);

    void render(cl_mem *corners, cl_mem *texture, cl_mem *FOV);

private:
    //Pixel level cube
    unsigned char* m_ucRed;
    unsigned char* m_ucGreen;
    unsigned char* m_ucBlue;

    cl_mem *m_clCubeDstSorted;
};

#endif // I3CPIXELCUBE_H
