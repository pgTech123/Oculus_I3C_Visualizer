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


bool boundingRectComputed(int id)
{
    return false;
}

bool isInBoundingRect(int4 boundingRect, int2 imgCoord)
{
    return true;
}

__kernel void render(__write_only image2d_t resultTexture,
                     __global int *numberOfLevels,
                     __global float3 *pixels,
                     __global uchar *references,
                     __global float3 *cornersArray,
                     __global int4 *boundingRectArray,
                     __global int *childId_memStatusBit,
                     __global int *FOV)
{
    //Get pixel we're working on
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float4 pixelValue;

    //Id of the cube we're currently analysing
    int cubeId = 0;
    int stackStandByCube[16];
    uchar2 stackPointer = 0;

    for(int level = numberOfLevels[0]; level >= 0; level--)
    {
        int4 boundingRect;

        if(!boundingRectComputed(cubeId)){
            //Compute subcorners
            //Compute bounding rect
            //Push data to global memory
        }
        else{
            //Get bounding rect
        }

        if(!isInBoundingRect(boundingRect, coord)){
            //Stop the loop and fill the pixel with black
            pixelValue = (float4)(0.0, 0.0, 0.0, 1.0);
            break;
        }
        else if(level == 0){
            //Go and take the appropriate pixel value
            pixelValue = (float4)(pixels[get_global_id(1)], 1.0);    //DEBUG
        }
        else{
            //update cubeId, ...
        }
    }

    //Write Image
    write_imagef(resultTexture, coord, pixelValue);
}


__kernel void clearMemoryBit(__global int *childId_memStatusBit)
{
    //TODO: Set memory bit to 0
}
