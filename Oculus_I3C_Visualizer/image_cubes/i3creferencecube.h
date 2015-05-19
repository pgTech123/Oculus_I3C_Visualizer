/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CPixelCube.h
 * Creation :   May 14th 2015
 * Purpose  :   Represent a Cube of references
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class represent a cube storing up to 8 references to
 * other cubes.
 * *********************************************************/

#ifndef I3CREFERENCECUBE_H
#define I3CREFERENCECUBE_H

#include <iostream>
#include <CL/cl.h>
#include "i3ccube.h"



class I3CReferenceCube : public I3CCube
{
public:
    I3CReferenceCube(cl_context *context,
                     cl_command_queue *commandQueue,
                     cl_uchar *uc_cubeDstSorted,
                     cl_kernel *computeChildCornersKernel);  //Array of size 8
    ~I3CReferenceCube();

    void addReferenceCube(unsigned char ucMap, I3CCube** p_ChildCubeRef);

    void render(cl_mem *corners);
    void render(Coordinate corners[8]);

private:
    void computeSubcorners();
    void renderChildIfZPositive(unsigned char cubeId);

private:
    //Reference cube
    I3CCube** m_pArrChildCubes;

    //GPU implementation
    cl_uchar *m_ptrArrCubeDstSorted;
    cl_mem m_clChildCorners[8];
    cl_kernel *m_clComputeChildCornersKernel;

    //CPU implementation
    Coordinate m_fArrSubcorners[27];
};

#endif // I3CREFERENCECUBE_H
