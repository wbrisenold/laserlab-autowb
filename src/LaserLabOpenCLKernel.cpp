// LaserLabOpenCLKernel.cpp — OpenCL implementation of the LaserLab grade.
// Includes shared kernel source and provides OpenCL-specific entry point.
// Verified against CPU reference (laserlab_core.h) by test_laserlab_opencl (parity ~2e-5).

#include <OpenCL/cl.h>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "LaserLabParams.h"
#include "LaserLabKernelSourceStr.h"

static cl_context g_clContext = nullptr;
static cl_command_queue g_clQueue = nullptr;
static cl_program g_clProgram = nullptr;
static cl_kernel g_clKernel = nullptr;
static cl_device_id g_clDevice = nullptr;

static bool initOpenCL(void* p_CmdQ) {
    if (g_clProgram) return true;

    cl_int err;
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, nullptr);
    if (err != CL_SUCCESS) { fprintf(stderr, "clGetPlatformIDs failed: %d\n", err); return false; }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &g_clDevice, nullptr);
    if (err != CL_SUCCESS) { fprintf(stderr, "clGetDeviceIDs failed: %d\n", err); return false; }

    if (p_CmdQ) {
        g_clQueue = (cl_command_queue)p_CmdQ;
        clRetainCommandQueue(g_clQueue);
        cl_command_queue_properties props;
        clGetCommandQueueInfo(g_clQueue, CL_QUEUE_CONTEXT, sizeof(cl_context), &g_clContext, nullptr);
        clRetainContext(g_clContext);
    } else {
        g_clContext = clCreateContext(nullptr, 1, &g_clDevice, nullptr, nullptr, &err);
        if (!g_clContext) { fprintf(stderr, "clCreateContext failed: %d\n", err); return false; }
        g_clQueue = clCreateCommandQueue(g_clContext, g_clDevice, 0, &err);
        if (!g_clQueue) { fprintf(stderr, "clCreateCommandQueue failed: %d\n", err); return false; }
    }

    const char* src = kLaserLabKernelSource;
    size_t srcLen = strlen(src);
    g_clProgram = clCreateProgramWithSource(g_clContext, 1, &src, &srcLen, &err);
    if (!g_clProgram) { fprintf(stderr, "clCreateProgramWithSource failed: %d\n", err); return false; }

    err = clBuildProgram(g_clProgram, 1, &g_clDevice, "-cl-std=CL2.0", nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t logSize; clGetProgramBuildInfo(g_clProgram, g_clDevice, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        char* log = (char*)malloc(logSize + 1);
        clGetProgramBuildInfo(g_clProgram, g_clDevice, CL_PROGRAM_BUILD_LOG, logSize, log, nullptr);
        log[logSize] = 0;
        fprintf(stderr, "clBuildProgram failed:\n%s\n", log);
        free(log);
        return false;
    }

    g_clKernel = clCreateKernel(g_clProgram, "LaserLabKernel", &err);
    if (!g_clKernel) { fprintf(stderr, "clCreateKernel failed: %d\n", err); return false; }

    return true;
}

void RunOpenCLLaserLab(void* p_CmdQ, int p_Width, int p_Height,
                       const LaserLabParams& p_Params,
                       const float* p_Src, float* p_Dst) {
    if (!initOpenCL(p_CmdQ)) return;

    cl_int err;
    size_t bytes = (size_t)p_Width * p_Height * 4 * sizeof(float);

    size_t global[2] = { (size_t)p_Width, (size_t)p_Height };
    size_t local[2] = { 16, 16 };

    cl_mem paramsBuf = clCreateBuffer(g_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(LaserLabParams), (void*)&p_Params, &err);
    cl_mem wBuf = clCreateBuffer(g_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (void*)&p_Width, &err);
    cl_mem hBuf = clCreateBuffer(g_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), (void*)&p_Height, &err);
    cl_mem srcBuf = clCreateBuffer(g_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, bytes, (void*)p_Src, &err);
    cl_mem dstBuf = clCreateBuffer(g_clContext, CL_MEM_WRITE_ONLY, bytes, nullptr, &err);

    err = clSetKernelArg(g_clKernel, 0, sizeof(cl_mem), &paramsBuf);
    err |= clSetKernelArg(g_clKernel, 1, sizeof(cl_mem), &wBuf);
    err |= clSetKernelArg(g_clKernel, 2, sizeof(cl_mem), &hBuf);
    err |= clSetKernelArg(g_clKernel, 3, sizeof(cl_mem), &srcBuf);
    err |= clSetKernelArg(g_clKernel, 4, sizeof(cl_mem), &dstBuf);
    if (err != CL_SUCCESS) { fprintf(stderr, "clSetKernelArg failed: %d\n", err); goto cleanup; }

    err = clEnqueueNDRangeKernel(g_clQueue, g_clKernel, 2, nullptr, global, local, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) { fprintf(stderr, "clEnqueueNDRangeKernel failed: %d\n", err); goto cleanup; }

    err = clEnqueueReadBuffer(g_clQueue, dstBuf, CL_TRUE, 0, bytes, (void*)p_Dst, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) { fprintf(stderr, "clEnqueueReadBuffer failed: %d\n", err); }

cleanup:
    clReleaseMemObject(paramsBuf);
    clReleaseMemObject(wBuf);
    clReleaseMemObject(hBuf);
    clReleaseMemObject(srcBuf);
    clReleaseMemObject(dstBuf);
}
