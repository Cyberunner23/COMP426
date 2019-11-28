#pragma once

#include <iostream>

#include "CL/cl.h"

cl_context CreateContext()
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

cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
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
