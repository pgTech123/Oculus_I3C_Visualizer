/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   cl_program.cl
 * Creation :   May 14th 2015
 * Purpose  :   Code that runs on the GPU
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This file contains all the OpenCL code that runs on the
 * GPU to optimize the rendering.
 * *********************************************************/

bool boundingRectComputed(int childId_memStatusBit)
{
    //Check memStatusBit in |childId_memStatusBit|
    if((childId_memStatusBit & 0x80000000) != 0){
        return true;
    }
    return false;
}

bool lockIfNotAlready(__global int* childId_memStatusBit, int id)
{
    __global int* address = &childId_memStatusBit[id];
    int unlockedValue = (childId_memStatusBit[id] & 0xBFFFFFFF);
    int lockedValue = (childId_memStatusBit[id] | 0x40000000);

    int old = atomic_cmpxchg(address, unlockedValue, lockedValue);
    if((old & 0x40000000) == 0){
        return true;    //Was not lock but is now
    }
    return false;
}

bool isInBoundingRect(int4 boundingRect, int2 imgCoord)
{
    //BoundingRect order: minx, maxx, miny, maxy
    if( boundingRect.x < imgCoord.x &&  /*minx < x*/
        boundingRect.y > imgCoord.x &&  /*maxx > x*/
        boundingRect.z < imgCoord.y &&  /*miny < y*/
        boundingRect.w > imgCoord.y     /*maxy > y*/)
    {
        return true;
    }
    return false;
}

int getChildId(int childId_memStatusBit)
{
    return (childId_memStatusBit & 0x1FFFFFFF);
}

void computeSubcorners(__global float3 *cornersArray, uchar reference, int cubeId, int childId, __global int4 *boundingRect, __global int *FOV, __global int *childId_memStatusBit)
{
    //Adjust id for the size of data
    int cubeIdCorners = 8*cubeId;

    float3 childCorners[27];
    //Order of computation of midpoints: see diagram...
    uchar outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    uchar midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    uchar computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    uchar midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    uchar computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};

    //Computation
    for(uchar i = 0; i < 8; i++){
        childCorners[outterCornersIndex[i]] = cornersArray[cubeIdCorners + i];
    }
    for(uchar i = 0; i < 12; i++){
        childCorners[midCornersIndex[i]] = (childCorners[computeMidWith[i][0]] + childCorners[computeMidWith[i][1]])/2;
    }
    for(uchar i = 0; i < 6; i++){
        childCorners[midFacesIndex[i]] = (childCorners[computeMidFaceWith[i][0]] + childCorners[computeMidFaceWith[i][1]])/2;
    }
    childCorners[13] = (childCorners[10] + childCorners[16])/2;

//debugOutput[0] = childCorners[13];

    //Write childs that exists
    int offset = 0;
    uchar childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};
    for(uchar i = 0; i < 8; i++){
        if((reference & (0x01 << i)) != 0){
            //Compute bounding rect for existing chidls only
            int minx = 100000;
            int maxx = 0;
            int miny = 100000;
            int maxy = 0;

            for(uchar corner = 0; corner < 8; corner++){

                //Compute bounding rech
                int x = (childCorners[childBaseCorner[i]+childBaseCorner[corner]].x / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[2];
                int y = (childCorners[childBaseCorner[i]+childBaseCorner[corner]].y / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[1];

                if(x < minx){
                    minx = x;
                }
                if(x > maxx){
                    maxx = x;
                }
                if(y < miny){
                    miny = y;
                }
                if(y > maxy){
                    maxy = y;
                }

                //Write child corners
                if(lockIfNotAlready(boundingRect, childId)){
                    cornersArray[(childId+offset)*8+corner] = childCorners[childBaseCorner[i]+childBaseCorner[corner]];

                    //Write bounding rect
                    boundingRect[childId+offset].x = minx;
                    boundingRect[childId+offset].y = maxx;
                    boundingRect[childId+offset].z = miny;
                    boundingRect[childId+offset].w = maxy;

                    atomic_or(&childId_memStatusBit[cubeId], 0x80000000);
                }
            }

            offset += 1;
        }
    }
}

__kernel void render(__write_only image2d_t resultTexture,
                     __global int *numberOfLevels,
                     __global float3 *pixels,
                     __global uchar *references,
                     __global float3 *cornersArray,
                     __global int4 *boundingRect,
                     __global int *childId_memStatusBit,
                     __global int *FOV,
                     __global int *topCubeId,
                     __global float3 *debugOutput/*,
                     __global uchar8 *renderingOrder*/)
{
    //Get pixel we're working on
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 pixelValue;

    //Id of the cube we're currently analysing
    int cubeId = topCubeId[0]-1;

    int stackStandByCube[64];   //May be limiting for large images(512 and more)... Adjust when time comes
    uchar8 stackPointer = 0;    //Must have the same number of bits than the size of |stackStandByCube|

    for(int level = numberOfLevels[0]; level >= 0; level--)
    {
        int childId = getChildId(childId_memStatusBit[cubeId]);

        //Compute child parameters if not already done
        if(!boundingRectComputed(childId_memStatusBit[cubeId])){
            computeSubcorners(cornersArray, references[cubeId], cubeId, childId, boundingRect, FOV, childId_memStatusBit);
        }

        //Check if the current pixel is outside the image(i.e. a black pixel)
        if(!isInBoundingRect(boundingRect[cubeId], coord)){
            //Stop the loop and fill the pixel with black
            pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);
            break;
        }
        else if(level == 0/*&& stack is empty*/){
            //Go and take the appropriate pixel value
            pixelValue = (float4)(pixels[get_global_id(1)], 1.0);    //DEBUG
        }
        else if(level == numberOfLevels[0]/*&& stack is empty*/){
            cubeId = childId;    //Get closest first
            //update cubeId, stack, ...
        }
    }

    //Write Image
    write_imagef(resultTexture, coord, pixelValue);
}


__kernel void clearMemoryBit(__global int *childId_memStatusBit)
{
    //Set memory bit to 0
    childId_memStatusBit[get_global_id(0)] = (childId_memStatusBit[get_global_id(0)] & 0x3FFFFFFF);
}


/*
//Compute subcorners
float3 childCorners[27];
int baseId = cubeId*8;

uchar outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
//Put corners we already know
for(uchar i = 0; i < 8; i++){
    childCorners[outterCornersIndex[i]] = cornersArray[baseId + i];
}
printf("%f", childCorners[0]);


//MiddleVertice
uchar midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
uchar computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                            {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
for(uchar i = 0; i < 12; i++){
    childCorners[midCornersIndex[i]] = (childCorners[computeMidWith[i][0]] + childCorners[computeMidWith[i][1]])/2;
}
//MiddleFace
uchar midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
uchar computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};
for(uchar i = 0; i < 6; i++){
    childCorners[midFacesIndex[i]] = (childCorners[computeMidFaceWith[i][0]] + childCorners[computeMidFaceWith[i][1]])/2;
}
//MiddlePoint
childCorners[13] = (childCorners[10] + childCorners[16])/2;
*/
