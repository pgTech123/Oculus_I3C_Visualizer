/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CPixelCube.cpp
 * Creation :   May 14th 2015
 * Purpose  :   Represent a Cube of references
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class represent a cube storing up to 8 references to
 * other cubes.
 * *********************************************************/

#include "i3creferencecube.h"

I3CReferenceCube::I3CReferenceCube(cl_mem *texture,
                                   cl_mem *FOV,
                                   cl_command_queue *commandQueue,
                                   unsigned char *uc_cubeDstSorted,
                                   cl_kernel *computeChildCornersKernel):I3CCube(texture, FOV, commandQueue)
{
    m_pArrChildCubes = NULL;
    m_ptrArrCubeDstSorted = uc_cubeDstSorted;
    m_clComputeChildCornersKernel = computeChildCornersKernel;
    for(int i = 0; i < 8; i++){
        m_clChildCorners[i] = NULL;
        //TODO: allocate cl_memory for corners
    }
}

I3CReferenceCube::~I3CReferenceCube()
{
    if(m_pArrChildCubes != NULL){
        delete[] m_pArrChildCubes;
    }
    for(int i = 0; i < 8; i++){
        if(m_clChildCorners[i] != NULL){
            clReleaseMemObject(m_clChildCorners[i]);
        }
    }
}

void I3CReferenceCube::addReferenceCube(unsigned char ucMap, I3CCube** p_ChildCubeRef)
{
    //Set hierarchy level
    m_iHierarchyLevel = p_ChildCubeRef[0]->getHierarchyLevel() + 1;

    //Create storage according to the map
    m_ucMap = ucMap;
    m_pArrChildCubes = new I3CCube*[8];

    //Set the reference for each specified child cube
    int i_Counter = 0;
    for(int i = 0; i < 8; i++)
    {
        if(m_ucMap & (0x01 << i)){
            m_pArrChildCubes[i] = (I3CCube*)p_ChildCubeRef[i_Counter];
            i_Counter++;
        }
    }
}

void I3CReferenceCube::render(cl_mem *corners)
{
    //Compute subcorners
    //TODO:
    //setarg
    //enqueueKernel

    //Render Childs
    for(int i = 0; i < 8; i++){     //WARNING: |i| Must NOT be higher than 8
        if((m_ucMap & (0x01 << (*(m_ptrArrCubeDstSorted+i))))){
            m_pArrChildCubes[*(m_ptrArrCubeDstSorted+i)]->render(&m_clChildCorners[*(m_ptrArrCubeDstSorted+i)]);
        }
    }
}
