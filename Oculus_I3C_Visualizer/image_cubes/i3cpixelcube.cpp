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

I3CPixelCube::I3CPixelCube(cl_mem *texture,     //Where |texture| is an array size 2
                           cl_mem *FOV,         //Where |FOV| is an array size 2
                           cl_command_queue *commandQueue,
                           cl_mem *cubeDstSorted):I3CCube(texture, FOV, commandQueue)
{
    m_ucRed = NULL;
    m_ucGreen = NULL;
    m_ucBlue = NULL;
    m_clCubeDstSorted = cubeDstSorted;
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

}
