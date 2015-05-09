/* *************************************************
 * Author   :   Pascal Gendron
 * Filename :   CL_Utils.h
 * Creation :   May 3rd 2015
 * Purpose  :   Abstract some OpenCL processes
 * Lisence  :   FreeBSD
 * *************************************************/

#include "cl_utils.h"
std::vector<GPU> getGPUAvailable()
{
    std::vector<GPU> GPU_found;
    GPU buffer_GPU;

    cl_int error = 0;
    cl_uint platAvail;
    cl_platform_id platform;
    cl_device_id device[10];
    cl_uint devices_found = 0;

    //Get platform
    error = clGetPlatformIDs(1, &platform, &platAvail);
    if (error != CL_SUCCESS) {
        return GPU_found;   //We return an empty vector
    }

    //Look for GPUs
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 10, device, &devices_found);
    if (error != CL_SUCCESS) {
       return GPU_found;   //We return an empty vector
    }

    //Query Info for each GPU
    for(unsigned int i = 0; i < devices_found; i++)
    {
        buffer_GPU.device_id = device[i];

        //WARNING: Errors are not caught here...
        clGetDeviceInfo(device[i], CL_DEVICE_NAME, sizeof(buffer_GPU.GPU_name), buffer_GPU.GPU_name, NULL);
        clGetDeviceInfo(device[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buffer_GPU.max_compute_units), &buffer_GPU.max_compute_units, NULL);
        clGetDeviceInfo(device[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buffer_GPU.max_frequency), &buffer_GPU.max_frequency, NULL);
        GPU_found.push_back(buffer_GPU);
    }
    return GPU_found;
}

Sources_OCL loadCLSources(char* filename, unsigned int max_length)
{
    Sources_OCL sources;

    FILE *fp;
    fp = fopen(filename, "r");
    if (!fp) {
        sources.success = false;
        return sources;
    }

    sources.sources = (char*)malloc(max_length);
    sources.source_size = fread(sources.sources, 1, max_length, fp);
    fclose(fp);

    sources.success = true;
    return sources;
}

GPU findBestGPU(std::vector<GPU> gpus)
{
    GPU bestGPU;
    int bestGPUFactor = 0;

    //Case that no GPUs in the vector
    if(gpus.size() < 0){
        bestGPU.GPU_name[0] = '\0';
        bestGPU.max_compute_units = 0;
        bestGPU.max_frequency = 0;
        bestGPU.device_id = NULL;
        return bestGPU;
    }

    //Find best GPU based on the multiplication of compute units and frequency.
    for(unsigned int i = 0; i < gpus.size(); i++){
        int gpuPerformanceFactor = gpus[i].max_compute_units * gpus[i].max_frequency;
        if(gpuPerformanceFactor > bestGPUFactor){
            bestGPU = gpus[i];
        }
    }

    return bestGPU;
}

cl_context createContexOnGPU(GPU gpu)
{
    cl_int error;
    cl_context context = clCreateContext(0, 1, &gpu.device_id, NULL, NULL, &error);
    //TODO: find a way to catch callback to detect errors while exec
    if (error != CL_SUCCESS) {
        return NULL;
    }
    return context;
}

cl_command_queue createCommandQueue(GPU gpu, cl_context context)
{
    cl_int error;
    cl_command_queue queue = clCreateCommandQueue(context, gpu.device_id, 0, &error);
    if (error == CL_SUCCESS) {
       return queue;
    }
    return  NULL;
}

int runProgramOnDevice(GPU gpu, cl_context context, char* sourcePath)
{
    cl_int error;
    Sources_OCL sources = loadCLSources(sourcePath);
    if(sources.success){
        //Create program
        cl_program program = clCreateProgramWithSource(context,  1, (const char **)&sources.sources,
                                                       (const size_t *)&sources.source_size, &error);
        if (error != CL_SUCCESS) {
           return ERROR_CREATING_PROGRAM;
        }

        //Compile Program
        error = clBuildProgram(program, 1, &gpu.device_id, NULL, NULL, NULL);
        if (error == CL_SUCCESS) {
           return SUCCESS;
        }
    }
    return WRONG_SOURCE_PATH;
}
