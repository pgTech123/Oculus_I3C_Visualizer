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

I3CReferenceCube::I3CReferenceCube(cl_context *context,
                                   cl_command_queue *commandQueue,
                                   unsigned char *uc_cubeDstSorted,
                                   cl_kernel *computeChildCornersKernel):I3CCube(context, commandQueue)
{
    m_pArrChildCubes = NULL;
    m_ptrArrCubeDstSorted = uc_cubeDstSorted;
    m_clComputeChildCornersKernel = computeChildCornersKernel;
    for(int i = 0; i < 8; i++){
        m_clChildCorners[i] = clCreateBuffer(*m_clContext, CL_MEM_READ_WRITE, 3*27*sizeof(float), NULL, NULL);
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

void I3CReferenceCube::render(cl_mem *corners, cl_mem *texture, cl_mem *FOV)
{
    //Set parent

    //Set Childs

    //Compute subcorners
    /*size_t workItems = 27;
    error = clEnqueueNDRangeKernel(*m_clCommandQueue, *computeChildCornersKernel, 1, NULL,
                                   &workItems , NULL, 0, NULL, NULL);*/

    //Render Childs
    for(int i = 0; i < 8; i++){     //WARNING: |i| Must NOT be higher than 8
        if((m_ucMap & (0x01 << (*(m_ptrArrCubeDstSorted+i))))){
            m_pArrChildCubes[*(m_ptrArrCubeDstSorted+i)]->render(&m_clChildCorners[*(m_ptrArrCubeDstSorted+i)],
                                                                 texture, FOV);
        }
    }
}
