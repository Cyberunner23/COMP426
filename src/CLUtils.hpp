#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <dns_sd.h>

#include "CL/cl.h"

#include "BallUtils.hpp"

struct CLState
{
    cl_context CTX;
    cl_command_queue CommandQueue;
    cl_program KernelProgram;
    cl_kernel BallUpdateKernel;
    cl_kernel BallCollisionKernel;
};

struct CLBallState
{
    cl_mem MassBuf;
    cl_mem RadiusBuf;
    cl_mem PositionBuf;
    cl_mem VelocityBuf;
    unsigned int Count;

    cl_mem CountBuf;
    cl_mem GravityBuf;
    cl_mem DeltaTBuf;
    cl_mem WinSizeBuf;
};

cl_context CreateCtx()
{

    cl_uint numPlatforms;
    cl_platform_id platformID;

    cl_int errCode = clGetPlatformIDs(1, &platformID, &numPlatforms);
    if (errCode != CL_SUCCESS || numPlatforms <= 0)
    {
        std::cerr << "Cannot find any OpenCL platforms." << std::endl;
        return nullptr;
    }

    cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties) platformID, 0 };
    cl_context context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &errCode);
    if (errCode != CL_SUCCESS)
    {
        std::cout << "Could not create GPU context, trying CPU..." << std::endl;
        context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_CPU, nullptr, nullptr, &errCode);
        if (errCode != CL_SUCCESS)
        {
            std::cerr << "Failed to create an OpenCL GPU or CPU context." << std::endl;
            return nullptr;
        }
    }

    return context;
}

cl_command_queue CreateCmdQueue(cl_context context, cl_device_id *device)
{

    size_t deviceBufferSize = -1;
    cl_int errCode = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, nullptr, &deviceBufferSize);
    if (errCode != CL_SUCCESS)
    {
        std::cerr << "Failed call to clGetContextInfo()";
        return nullptr;
    }

    if (deviceBufferSize <= 0)
    {
        std::cerr << "No devices available.";
        return nullptr;
    }

    cl_device_id* devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errCode = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, nullptr);
    if (errCode != CL_SUCCESS)
    {
        delete [] devices;
        std::cerr << "Failed to get device IDs";
        return nullptr;
    }

    cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, nullptr);
    if (commandQueue == nullptr)
    {
        delete [] devices;
        std::cerr << "Failed to create commandQueue for device 0";
        return nullptr;
    }

    *device = devices[0];
    delete [] devices;
    return commandQueue;
}

cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
    std::ifstream kernelFile(fileName, std::ios::in);
    if (!kernelFile.is_open())
    {
        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
        return nullptr;
    }

    std::ostringstream oss;
    oss << kernelFile.rdbuf();

    std::string srcStdStr = oss.str();
    const char *srcStr = srcStdStr.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&srcStr, nullptr, nullptr);

    if (program == nullptr)
    {
        std::cerr << "Failed to create CL program from source." << std::endl;
        return nullptr;
    }

    cl_int errCode = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (errCode != CL_SUCCESS)
    {
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, nullptr);
        std::cerr << "Error in kernel: " << std::endl;
        std::cerr << buildLog;
        clReleaseProgram(program);
        return nullptr;
    }

    return program;
}

bool CreateKernels(CLState& state)
{
    state.BallUpdateKernel = clCreateKernel(state.KernelProgram, "update_ball", nullptr);
    state.BallCollisionKernel = clCreateKernel(state.KernelProgram, "handle_collisions", nullptr);
    if (!state.BallUpdateKernel || !state.BallCollisionKernel)
    {
        std::cerr << "Failed to create kernel" << std::endl;
        return false;
    }
    return true;
}

bool AllocateMemObjects(cl_context context, BallState &state, CLBallState& clState)
{
    clState.MassBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, state.Count * sizeof(cl_float), state.Mass, nullptr);
    clState.RadiusBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, state.Count * sizeof(cl_uint), state.Radius, nullptr);
    clState.PositionBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, state.Count * sizeof(cl_float2), state.Position, nullptr);
    clState.VelocityBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, state.Count * sizeof(cl_float2), state.Velocity, nullptr);

    clState.CountBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint), (cl_uint*)&state.Count, nullptr);
    clState.GravityBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float), (cl_float*)&gravity, nullptr);
    clState.WinSizeBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint), (cl_uint*)&WinSize, nullptr);

    cl_float f = 0;
    clState.DeltaTBuf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float), &f, nullptr);

    clState.Count = state.Count;

    if (clState.MassBuf == nullptr
    || clState.RadiusBuf == nullptr
    || clState.PositionBuf == nullptr
    || clState.VelocityBuf == nullptr
    || clState.CountBuf == nullptr
    || clState.GravityBuf == nullptr
    || clState.DeltaTBuf == nullptr
    || clState.WinSizeBuf == nullptr)
    {
        std::cerr << "Failed to create CL mem objects" << std::endl;
        return false;
    }

    return true;
}


void Deallocate(CLState& clState, CLBallState& clBallState)
{
    clReleaseMemObject(clBallState.MassBuf);
    clReleaseMemObject(clBallState.RadiusBuf);
    clReleaseMemObject(clBallState.PositionBuf);
    clReleaseMemObject(clBallState.VelocityBuf);

    clReleaseMemObject(clBallState.CountBuf);
    clReleaseMemObject(clBallState.GravityBuf);
    clReleaseMemObject(clBallState.WinSizeBuf);
    clReleaseMemObject(clBallState.DeltaTBuf);

    if (clState.CommandQueue)
    {
        clReleaseCommandQueue(clState.CommandQueue);
    }

    if (clState.BallUpdateKernel)
    {
        clReleaseKernel(clState.BallUpdateKernel);
    }

    if (clState.BallCollisionKernel)
    {
        clReleaseKernel(clState.BallCollisionKernel);
    }

    if (clState.KernelProgram)
    {
        clReleaseProgram(clState.KernelProgram);
    }

    if (clState.CTX)
    {
        clReleaseContext(clState.CTX);
    }
}

int InitOpenCL(BallState& ballState, CLBallState& clBallState, CLState& clState)
{
    clState.CTX = CreateCtx();
    if (!clState.CTX)
    {
        return -1;
    }

    cl_device_id deviceID = nullptr;
    clState.CommandQueue = CreateCmdQueue(clState.CTX, &deviceID);
    if (!clState.CommandQueue)
    {
        Deallocate(clState, clBallState);
        return -2;
    }

    clState.KernelProgram = CreateProgram(clState.CTX, deviceID, "BallLogic.cl");
    if (!clState.KernelProgram)
    {
        Deallocate(clState, clBallState);
        return -3;
    }

    if (!CreateKernels(clState))
    {
        Deallocate(clState, clBallState);
        return -4;
    }

    if (!AllocateMemObjects(clState.CTX, ballState, clBallState))
    {
        Deallocate(clState, clBallState);
        return -5;
    }

    return 0;
}

bool SetBallUpdateKernelParamsInit(CLBallState& clBallState, CLState& clState, cl_float& gravity, cl_uint& WinSize)
{
    cl_int errCode = clSetKernelArg(clState.BallUpdateKernel, 0, sizeof(cl_mem), &clBallState.MassBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 1, sizeof(cl_mem), &clBallState.RadiusBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 2, sizeof(cl_mem), &clBallState.PositionBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 3, sizeof(cl_mem), &clBallState.VelocityBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 4, sizeof(cl_mem), &clBallState.GravityBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 5, sizeof(cl_mem), &clBallState.DeltaTBuf);
    errCode |= clSetKernelArg(clState.BallUpdateKernel, 6, sizeof(cl_mem), &clBallState.WinSizeBuf);

    return errCode == CL_SUCCESS;
}

bool BallUpdateBufferUpdate(CLState& clState, CLBallState& clBallState, float& deltaT)
{
    cl_event readEvent;

    if(clEnqueueWriteBuffer(clState.CommandQueue, clBallState.DeltaTBuf, CL_TRUE, 0, sizeof(cl_float), (cl_float*)&deltaT, 0, nullptr, &readEvent) != CL_SUCCESS)
    {
        return false;
    }

    return clWaitForEvents(1, &readEvent) == CL_SUCCESS;
}

bool BallUpdateBufferRead(CLState& clState, CLBallState& clBallState, float& deltaT)
{
    cl_event writeEvent;

    if (clEnqueueReadBuffer(clState.CommandQueue, clBallState.DeltaTBuf, CL_TRUE, 0, sizeof(cl_float), (void *)&deltaT, 0, NULL, &writeEvent) != CL_SUCCESS)
    {
        return false;
    }

    return clWaitForEvents(1, &writeEvent) == CL_SUCCESS;
}

bool SetBallCollisionKernelParamsInit(CLBallState& clBallState, CLState& clState, cl_uint& WinSize)
{
    cl_int errCode = clSetKernelArg(clState.BallCollisionKernel, 0, sizeof(cl_mem), &clBallState.MassBuf);
    errCode |= clSetKernelArg(clState.BallCollisionKernel, 1, sizeof(cl_mem), &clBallState.RadiusBuf);
    errCode |= clSetKernelArg(clState.BallCollisionKernel, 2, sizeof(cl_mem), &clBallState.PositionBuf);
    errCode |= clSetKernelArg(clState.BallCollisionKernel, 3, sizeof(cl_mem), &clBallState.VelocityBuf);
    errCode |= clSetKernelArg(clState.BallCollisionKernel, 4, sizeof(cl_mem), &clBallState.CountBuf);
    errCode |= clSetKernelArg(clState.BallCollisionKernel, 5, sizeof(cl_mem), &clBallState.WinSizeBuf);

    return errCode == CL_SUCCESS;
}

bool RunKernels(CLState& state, CLBallState& clBallState)
{
    size_t localWorkSize = 1;
    cl_event runEvents[2];
    cl_int errCode = clEnqueueNDRangeKernel(state.CommandQueue, state.BallUpdateKernel, 1, nullptr, (size_t*)&clBallState.Count, &localWorkSize, 0, nullptr, &runEvents[0]);
    errCode = clEnqueueNDRangeKernel(state.CommandQueue, state.BallCollisionKernel, 1, nullptr, (size_t*)&clBallState.Count, &localWorkSize, 0, nullptr, &runEvents[1]);

    return errCode == CL_SUCCESS && clWaitForEvents(2, runEvents) == CL_SUCCESS;
}

bool ReadPositionBuffer(BallState& ballState, CLBallState& clBallState, CLState& clState)
{
    cl_event readEvent;

    if(clEnqueueReadBuffer(clState.CommandQueue, clBallState.PositionBuf, CL_TRUE, 0, clBallState.Count * sizeof(cl_float2), ballState.Position, 0, nullptr, &readEvent) != CL_SUCCESS)
    {
        return false;
    }

    return clWaitForEvents(1, &readEvent) == CL_SUCCESS;
}











