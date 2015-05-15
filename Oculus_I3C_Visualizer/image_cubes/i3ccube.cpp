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


//TODO: TEXTURE AND FOV Can be exclusive to pixelCube: no need to be here...

#include "i3ccube.h"

I3CCube::I3CCube(cl_mem *texture,
                 cl_mem *FOV,
                 cl_command_queue *commandQueue)
{
    m_clTexture = texture;
    m_clFOV = FOV;
    m_clCommandQueue = commandQueue;
}


int I3CCube::getHierarchyLevel()
{
    return m_iHierarchyLevel;
}

