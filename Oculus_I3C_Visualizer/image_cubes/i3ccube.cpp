/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CCube.h
 * Creation :   May 14th 2015
 * Purpose  :   Represent a single Cube composing the Image
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class(abstract) represent a single cube composing
 * the image. This cube can either be a pixel cube or a
 * reference cube.
 * *********************************************************/


#include "i3ccube.h"

//TODO: optimize this in assembly
void Coordinate::fromMidCoord(Coordinate coord1, Coordinate coord2)
{
    this->x = (coord1.x + coord2.x) / 2;
    this->y = (coord1.y + coord2.y) / 2;
    this->z = (coord1.z + coord2.z) / 2;
}

I3CCube::I3CCube(cl_context *context,
                 cl_command_queue *commandQueue)
{
    m_clContext = context;
    m_clCommandQueue = commandQueue;
}

I3CCube::~I3CCube()
{
}

int I3CCube::getHierarchyLevel()
{
    return m_iHierarchyLevel;
}

