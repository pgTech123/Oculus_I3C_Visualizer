/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   cl_program.cl
 * Creation :   May 14th 2015
 * Purpose  :   Rendering code
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This file contains 2 kernels:
 *  - clearMemoryBit()
 *  - render()
 *
 * Clear Memory Bit:
 *  Clear memory bit reset the bits that mark the cubes corners/boundaries as computed.
 *  Must be called between each frame.
 *
 * Render:
 *  Each instance of |render()| compute the value of a specific pixel.
 *  The render kernel should be launched width*height times.
 * *********************************************************/

/* *********************************************************************************************
 * KNOWN BUGS / TODO LIST:
 * - (HIGH PRIORITY BUG) Some cube not displayed. Depends partly on head orientation. Reason:?
 * - (HIGH PRIORITY BUG) When the image is totally not seen: full screen with random colors
 * - (MEDUIM PRIORITY TODO) Clean the code
 * - (MEDIUM PRIORITY TODO) Optimize the code
 * **********************************************************************************************/


#define STACK_SIZE      88       //Should be enough for any images of 2048 px or less (not tested)


// --------  FUNCTIONS DECLARATION  --------

bool boundingRectComputed(int childId_memStatusBit);
bool lockIfNotAlready(__global int* childId_memStatusBit, int id);
bool isInBoundingRect(int4 boundingRect, int2 imgCoord);
int getChildId(int childId_memStatusBit);
int numberOfHighBitsWithinBoundary(uchar map, int boundary);

void computeSubcorners(uchar reference,
                       int cubeId,
                       __global float3 *cornersArray,
                       __global int4 *boundingRect,
                       __global int *FOV,
                       __global int *childId_memStatusBit,
                       __global float3 *debugOutput);

void pushStack(__private int *levelStack,   /*input/output*/
               __private int *idStack,      /*input/output*/
               __private int *stackCursor,  /*input/output*/
               int levelToInsert,           /*input*/
               int idToInsert);              /*input*/

void popStack(__private int *levelStack,    /*input*/
              __private int *idStack,       /*input*/
              __private int *stackCursor,   /*input/output*/
              __private int *level,         /*output*/
              __private int *cubeId);       /*output*/



// --------  KERNELS DEFINITION  --------

__kernel void clearMemoryBit(__global int *childId_memStatusBit)
{
    atomic_and(childId_memStatusBit+get_global_id(0), 0x3FFFFFFF);  //Reset status bits
}

__kernel void render(__write_only image2d_t texture,
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
    int2 pixelCoord = (int2)(get_global_id(0), get_global_id(1));

    int currentCubeId = topCubeId[0]-1;

    //Stack
    int levelStack[STACK_SIZE];
    int idStack[STACK_SIZE];
    int stackCursor = -1;

    for(int level = numberOfLevels[0]; level >= 0; level--)
    {
        if(level == 0){
            pixelValue = (float4)(pixels[currentCubeId], 1.0);
            break;
        }

        //Check if the current pixel is NOT within the current cube boundaries
        if(!isInBoundingRect(boundingRect[currentCubeId], pixelCoord)){
            if(stackCursor < 0){                            //No other possible path
                pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);  //Not seen => black pixel
                break;
            }
            else{
                //Pop stack (try another path)
                popStack(&levelStack, &idStack, &stackCursor, &level, &currentCubeId);
                continue;
            }
        }

        //Compute child corners (and make them available to every other work unit)
        if((childId_memStatusBit[currentCubeId] & 0x80000000) == 0 &&
                lockIfNotAlready(childId_memStatusBit, currentCubeId))    //if not written and not locked
        {
            computeSubcorners(references[currentCubeId], currentCubeId, cornersArray,
                              boundingRect, FOV, childId_memStatusBit, debugOutput);
        }
        //Wait for the data to be set before to go any further
        while((childId_memStatusBit[currentCubeId] & 0x80000000) == 0){}


        //Push children on the stack (and order their position on the stack)
        uchar cubeMap = references[currentCubeId];
        int childIdBase = getChildId(childId_memStatusBit[currentCubeId]);
        int offset;
        for(int i = 7; i >= 0; i--){
            if((cubeMap & (0x01 << renderingOrder[i])) != 0){
                offset = numberOfHighBitsWithinBoundary(cubeMap, renderingOrder[i]);
                pushStack(&levelStack, &idStack, &stackCursor, level, childIdBase+offset);
            }
        }
        //Pop stack
        popStack(&levelStack, &idStack, &stackCursor, &level, &currentCubeId);
    }

    //Write Image
    write_imagef(texture, pixelCoord, pixelValue);
}



// -------- FUNCTIONS DEFINITION  --------

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

bool isInBoundingRect(const int4 boundingRect, const int2 imgCoord)
{
    //BoundingRect order: minx, maxx, miny, maxy
    if( boundingRect.x <= imgCoord.x &&  /*minx <= x*/
        boundingRect.y >= imgCoord.x &&  /*maxx >= x*/
        boundingRect.z <= imgCoord.y &&  /*miny <= y*/
        boundingRect.w >= imgCoord.y)    /*maxy >= y*/
    {
        return true;
    }
    else{
        return false;
    }
}

int getChildId(int childId_memStatusBit)
{
    return (childId_memStatusBit & 0x1FFFFFFF);     //Remove status bits
}

int numberOfHighBitsWithinBoundary(uchar map, int boundary)
{
    int count = 0;
    uchar compareWith = 0x01;
    for(uchar i = 0; i < boundary; i++){
        if((map &compareWith) != 0){
            count ++;
        }
        compareWith = compareWith << 1;
    }
    return count;
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
    uchar childBaseCorner[8] = {0, 1, 10, 9, 3, 4, 13, 12};     //Order important
    uchar outterCornersIndex[8] = {0, 2, 20, 18, 6, 8, 26, 24};
    uchar midCornersIndex[12] = {1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25};
    uchar computeMidWith[12][2] = {{0,2}, {0,6}, {2,8}, {6,8}, {0,18}, {2,20},
                                {6,24}, {8,26}, {18,20}, {18,24}, {20,26}, {24,26}};
    uchar midFacesIndex[6] = {4, 10, 12, 14, 16, 22};
    uchar computeMidFaceWith[6][2] = {{1,7}, {1,19}, {3,21}, {5,23}, {15,17}, {19,25}};

    //Subcorners computation
    for(uint i = 0; i < 8; i++){
        childCorners[outterCornersIndex[i]] = cornersArray[cubeIdCorners + i];
    }
    for(uchar i = 0; i < 12; i++){
        childCorners[midCornersIndex[i]] = (childCorners[computeMidWith[i][0]] + childCorners[computeMidWith[i][1]])/2.0;
    }
    for(uchar i = 0; i < 6; i++){
        childCorners[midFacesIndex[i]] = (childCorners[computeMidFaceWith[i][0]] + childCorners[computeMidFaceWith[i][1]])/2.0;
    }
    childCorners[13] = (childCorners[10] + childCorners[16])/2.0;

    //Write cubes (8 potential children)
    for(uchar i = 0; i < 8; i++){
        //Check if child exist
        if((reference & (0x01 << i)) != 0){
            int minx = 100000;
            int maxx = 0;
            int miny = 100000;
            int maxy = 0;

            for(uchar corner = 0; corner < 8; corner++){
                //Write child corner
                cornersArray[(childId+offset)*8+corner] = childCorners[childBaseCorner[i]+childBaseCorner[corner]];

                //Compute bounding rect
                int x = (int)(childCorners[childBaseCorner[i]+childBaseCorner[corner]].x / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[2];
                int y = (int)(childCorners[childBaseCorner[i]+childBaseCorner[corner]].y / childCorners[childBaseCorner[i]+childBaseCorner[corner]].z) - FOV[1];

                //Bounding rect test
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
            }
            //Write bounding rect
            boundingRect[childId+offset].x = minx;
            boundingRect[childId+offset].y = maxx;
            boundingRect[childId+offset].z = miny;
            boundingRect[childId+offset].w = maxy;

            offset ++;
        }
    }
    atomic_or(childId_memStatusBit+cubeId, 0x80000000);     //Set "data available" flag
}

void pushStack(__private int *levelStack,   /*input/output*/
               __private int *idStack,      /*input/output*/
               __private int *stackCursor,  /*input/output*/
               int levelToInsert,           /*input*/
               int idToInsert)              /*input*/
{
    (*stackCursor)++;
    levelStack[*stackCursor] = levelToInsert;
    idStack[*stackCursor] = idToInsert;
}

void popStack(__private int *levelStack,    /*input*/
              __private int *idStack,       /*input*/
              __private int *stackCursor,   /*input/output*/
              __private int *level,         /*output*/
              __private int *cubeId)        /*output*/
{
    (*level) = levelStack[*stackCursor];
    (*cubeId) = idStack[*stackCursor];
    (*stackCursor)--;
}

