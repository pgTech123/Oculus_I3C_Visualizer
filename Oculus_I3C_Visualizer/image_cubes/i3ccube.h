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
    I3CCube(cl_context *context,
            cl_command_queue *commandQueue);

    int getHierarchyLevel();

    virtual void render(cl_mem *corners, cl_mem *texture, cl_mem *FOV) = 0;

protected:
    int m_iHierarchyLevel;
    unsigned char m_ucMap;

    cl_context *m_clContext;
    cl_command_queue *m_clCommandQueue;
};

#endif // I3CCUBE_H
