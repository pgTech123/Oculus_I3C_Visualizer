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
                                   cl_uchar *uc_cubeDstSorted,
                                   cl_kernel *computeChildCornersKernel):I3CCube(context, commandQueue)
{
    m_pArrChildCubes = NULL;
    m_ptrArrCubeDstSorted = uc_cubeDstSorted;
    m_clComputeChildCornersKernel = computeChildCornersKernel;
    for(int i = 0; i < 8; i++){
        m_clChildCorners[i] = clCreateBuffer(*m_clContext, CL_MEM_READ_WRITE, 8*sizeof(cl_float3), NULL, NULL);
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
    //Set Childs
   /* for(unsigned int i = 0; i < 8; i++){
        clSetKernelArg(*m_clComputeChildCornersKernel, i, sizeof(m_clChildCorners[i]), (void *)&m_clChildCorners[i]);
    }
    //Set parent
    clSetKernelArg(*m_clComputeChildCornersKernel, 8, sizeof(*corners), (void *)corners);
*/
    //Compute subcorners
    size_t workItems = 4;
    cl_int error = clEnqueueNDRangeKernel(*m_clCommandQueue, *m_clComputeChildCornersKernel, 1, NULL,
                           &workItems , NULL, 0, NULL, NULL);

    //Render Childs
    for(int i = 0; i < 8; i++){     //WARNING: |i| Must NOT be higher than 8
        if((m_ucMap & (0x01 << (*(m_ptrArrCubeDstSorted+i))))){
            m_pArrChildCubes[*(m_ptrArrCubeDstSorted+i)]->render(&m_clChildCorners[*(m_ptrArrCubeDstSorted+i)]);
        }
    }
}

void I3CReferenceCube::render(Coordinate corners[8])
{
    int outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    for(int i = 0; i < 8; i++){
        m_fArrSubcorners[outterCornersIndex[i]] = corners[i];
    }
    computeSubcorners();

    for(int i = 0; i < 8; i ++){
        if((m_ucMap & (0x01 << (*(m_ptrArrCubeDstSorted+i))))){
            renderChildIfZPositive((*(m_ptrArrCubeDstSorted+i)));
        }
    }
}

void I3CReferenceCube::computeSubcorners()
{
    //Compute Mid
    int midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    int computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                 {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    for(int i = 0; i < 12; i++){
        m_fArrSubcorners[midCornersIndex[i]].fromMidCoord(m_fArrSubcorners[computeMidWith[i][0]],
                                                          m_fArrSubcorners[computeMidWith[i][1]]);
    }

    //Compute mid face
    int midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    int computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};
    for(int i = 0; i < 6; i++){
        m_fArrSubcorners[midFacesIndex[i]].fromMidCoord(m_fArrSubcorners[computeMidFaceWith[i][0]],
                                                        m_fArrSubcorners[computeMidFaceWith[i][1]]);
    }

    //Compute center
    int cubeCenter[1] = {13};
    int computeFrom[1][2] = {{10,16}};
    m_fArrSubcorners[cubeCenter[0]].fromMidCoord(m_fArrSubcorners[computeFrom[0][0]],
                                                 m_fArrSubcorners[computeFrom[0][1]]);
}

void I3CReferenceCube::renderChildIfZPositive(unsigned char cubeId)
{
    int childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};
    Coordinate childCorners[8];

    childCorners[0] = m_fArrSubcorners[childBaseCorner[cubeId]];
    childCorners[1] = m_fArrSubcorners[childBaseCorner[cubeId]+1];
    childCorners[2] = m_fArrSubcorners[childBaseCorner[cubeId]+10];
    childCorners[3] = m_fArrSubcorners[childBaseCorner[cubeId]+9];
    childCorners[4] = m_fArrSubcorners[childBaseCorner[cubeId]+3];
    childCorners[5] = m_fArrSubcorners[childBaseCorner[cubeId]+4];
    childCorners[6] = m_fArrSubcorners[childBaseCorner[cubeId]+13];
    childCorners[7] = m_fArrSubcorners[childBaseCorner[cubeId]+12];

    //If all Z are negative, stop rendering: won't be seen
    for(int i = 0; i < 8; i++){
        if(childCorners[i].z > 0){
            break;
        }
        return; //All negative: stop rendering this branch
    }

    //TODO: Opencl Optimisation: check if cube fully hidden starting after x pixels written
    //if(!childIsHidden(childCorners)){
        m_pArrChildCubes[cubeId]->render(childCorners);
    //}
}
