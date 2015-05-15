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

#ifndef I3CCUBE_H
#define I3CCUBE_H

#include <CL/cl.h>

class I3CCube
{
public:
    I3CCube(cl_mem *texture,
            cl_mem *FOV,
            cl_command_queue *commandQueue);

    int getHierarchyLevel();

    virtual void render(cl_mem *corners) = 0;

protected:
    int m_iHierarchyLevel;
    unsigned char m_ucMap;

    cl_mem *m_clTexture;
    cl_mem *m_clFOV;
    cl_command_queue *m_clCommandQueue;
};

#endif // I3CCUBE_H
