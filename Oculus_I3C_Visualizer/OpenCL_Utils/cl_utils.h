/* *************************************************
 * Author   :   Pascal Gendron
 * Filename :   CL_Utils.h
 * Creation :   May 3rd 2015
 * Purpose  :   Abstract some OpenCL processes
 * Lisence  :   FreeBSD
 *
 * Description:
 * TODO...
 * *************************************************/

#ifndef CL_UTILS
#define CL_UTILS

#include <CL/cl.hpp>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS                 0
#define WRONG_SOURCE_PATH       601
#define COMPILATION_ERROR       602
#define ERROR_CREATING_PROGRAM  603

typedef struct{
    char GPU_name[100];
    unsigned int max_frequency;
    unsigned int max_compute_units;
    cl_device_id device_id;
}GPU;

typedef struct{
    char *sources;
    size_t source_size;
    bool success;
}Sources_OCL;

/*#ifdef __cplusplus
extern "C" {
#endif*/

// If an error occurs or no GPU are available, the vector will be empty
// limited to 10 GPU on 1 platform
std::vector<GPU> getGPUAvailable();

// If Sources_OCL.success is false, an error was encountered and the structure
// may not be properly set.
// By default: CL sources max 1 Mo
Sources_OCL loadCLSources(char* filename, unsigned int max_length=100000);

// Returns the GPU struct that seems to show the best spec
// This function should not be called if there is no elements in the vector
// Nevertheless, the function will return a fake NULL GPU if it happens.
GPU findBestGPU(std::vector<GPU> gpus);

// This function creates a context on the specified GPU
cl_context createContexOnGPU(GPU gpu);

// This function returns the command queue for teh specified device
cl_command_queue createCommandQueue(GPU gpu, cl_context context);

//This function will initiate
int runProgramOnDevice(GPU gpu, cl_context context, char* sourcePath);

/*#ifdef __cplusplus
}
#endif*/

#endif // CL_UTILS

