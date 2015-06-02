/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   cl_program.cl
 * Creation :   May 14th 2015
 * Purpose  :   Code that runs on the GPU
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This file contains all the OpenCL code that runs on the
 * GPU. This is the code that makes the rendering possible.
 * *********************************************************/


__kernel void clearMemoryBit(__global int *childId_memStatusBit)
{
    atomic_and(childId_memStatusBit+get_global_id(0), 0x1FFFFFFF);  //Reset status bits
}

bool boundingRectComputed(int childId_memStatusBit)
{
    if((childId_memStatusBit & 0x80000000) != 0){
        return true;    //Computed
    }
    return false;       //Not computed
}

bool lockIfNotAlready(__global int* childId_memStatusBit, int id)
{
    int unlockedValue = (childId_memStatusBit[id] & 0xBFFFFFFF);
    int lockedValue = (childId_memStatusBit[id] | 0x40000000);

    int old = atomic_cmpxchg(childId_memStatusBit+id, unlockedValue, lockedValue);
    if((old & 0x40000000) == 0){
        return true;    //Was not lock but is now
    }
    return false;       //Was already locked
}

bool isInBoundingRect(int4 boundingRect, int2 imgCoord)
{
    //BoundingRect order: minx, maxx, miny, maxy
    if( boundingRect.x <= imgCoord.x &&  /*minx <= x*/
        boundingRect.y >= imgCoord.x &&  /*maxx >= x*/
        boundingRect.z <= imgCoord.y &&  /*miny <= y*/
        boundingRect.w >= imgCoord.y     /*maxy >= y*/)
    {
        return true;
    }
    return false;
}

int getChildId(int childId_memStatusBit)
{
    return (childId_memStatusBit & 0x1FFFFFFF);     //Remove status bits
}

void computeSubcorners(uchar reference,
                       int cubeId,
                       __global float3 *cornersArray,
                       __global int4 *boundingRect,
                       __global int *FOV,
                       __global int *childId_memStatusBit,
                       __global float3 *debugOutput)
{
    int childId = getChildId(childId_memStatusBit[cubeId]);

    int offset = 0;
    int cubeIdCorners = 8*cubeId;
    float3 childCorners[27];

    //Order of computation: see diagram cause without it, this is really hard to understand
    uchar childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};
    uchar outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    uchar midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    uchar computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    uchar midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    uchar computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};

    //Computation
    for(uint i = 0; i < 8; i++){
        childCorners[outterCornersIndex[i]] = cornersArray[cubeIdCorners + i];
    }
    for(uchar i = 0; i < 12; i++){
        childCorners[midCornersIndex[i]] = (childCorners[computeMidWith[i][0]] + childCorners[computeMidWith[i][1]])/2;
    }
    for(uchar i = 0; i < 6; i++){
        childCorners[midFacesIndex[i]] = (childCorners[computeMidFaceWith[i][0]] + childCorners[computeMidFaceWith[i][1]])/2;
    }
    childCorners[13] = (childCorners[10] + childCorners[16])/2;


    //printf("a ");   //TODO: figure out why more and more a printed as program runs...

    //Write cubes (8 potential children)
    for(uchar i = 0; i < 8; i++){
        //Check if child exist
        if((reference & (0x01 << i)) != 0){
            int minx = 100000;
            int maxx = 0;
            int miny = 100000;
            int maxy = 0;

            for(uchar corner = 0; corner < 8; corner++){
                //Write child corners
                cornersArray[(childId+offset)*8+corner] = childCorners[childBaseCorner[i]+childBaseCorner[corner]];

                //Compute bounding rech
                int x = (int)(childCorners[childBaseCorner[i]+childBaseCorner[corner]].x / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[2];
                int y = (int)(childCorners[childBaseCorner[i]+childBaseCorner[corner]].y / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[1];

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

                //Write bounding rect
                boundingRect[childId+offset].x = minx;
                boundingRect[childId+offset].y = maxx;
                boundingRect[childId+offset].z = miny;
                boundingRect[childId+offset].w = maxy;
            }
            offset += 1;
        }
    }
    atomic_or(childId_memStatusBit+cubeId, 0x80000000);
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
                     __global uchar *renderingOrder,
                     __global float3 *debugOutput)
{
    float4 pixelValue;
    int2 coord = (int2)(get_global_id(0), get_global_id(1));    //Coordinates of the current pixel
    int cubeId = topCubeId[0]-1;                                //Start with the biggest cube

    int levelStack[88]; //Guarantied to work with images of 2048 px or less
    int idStack[88];    //Guarantied to work with images of 2048 px or less
    int stackCursor = -1;

    //We go down to smaller cube until we reach pixel cubes
    for(int level = numberOfLevels[0]; level >= 0; level--)
    {
        //------    CHILD COMPUTATION     -------
        if((childId_memStatusBit[cubeId] & 0x80000000) == 0 &&
                lockIfNotAlready(childId_memStatusBit, cubeId))    //Not written and not locked
        {
            computeSubcorners(references[cubeId], cubeId, cornersArray,
                              boundingRect, FOV, childId_memStatusBit, debugOutput);
        }
        //Wait for the data to be set before to go any further
        while(!boundingRectComputed(childId_memStatusBit[cubeId])){}

        //------    CHECK IF PIXEL IS NOT SEEN     --------
        if(!isInBoundingRect(boundingRect[cubeId], coord)){
            if(stackCursor < 0){
                pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);      //Not seen => black pixel
                break;
            }
            else{
                //Pop stack
                level = levelStack[stackCursor];
                cubeId = idStack[stackCursor];
                stackCursor--;
                continue;
            }
        }

        //------    PUSH CHILDREN ON THE STACK     -------
        uchar cubeMap = references[cubeId];
        int childIdBase = getChildId(childId_memStatusBit[cubeId]);
        int offset = 0;

        if(level > 1){
            for(int i = 0; i < 8; i++){
                if((cubeMap & (0x01 << renderingOrder[i])) != 0){ //TODO: Order in distance
                    //Push on stack
                    stackCursor++;
                    levelStack[stackCursor] = level;
                    idStack[stackCursor] = childIdBase + offset;
                    offset++;
                }
            }
            //Pop stack
            level = levelStack[stackCursor];
            cubeId = idStack[stackCursor];
            stackCursor--;
        }

        //-----     FILL PIXEL WITH THE APPROPRIATE COLOR   ------
        if(level == 0){
            pixelValue = (float4)(pixels[cubeId], 1.0);
            break;
        }
    }

    //Write Image
    write_imagef(resultTexture, coord, pixelValue);
}
