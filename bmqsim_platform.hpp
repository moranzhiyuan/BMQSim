#pragma once

// Keep the upstream CUDA call sites and their synchronization order.
#ifdef BMQSIM_USE_HIP
#include <hip/hip_runtime.h>
#ifdef BMQSIM_DTK_GRID_ABI
#include "bmqsim_dtk_driver_abi.h"
#endif
#include <hip/hip_cooperative_groups.h>

using cudaError = hipError_t;
using cudaError_t = hipError_t;
using cudaDeviceProp = hipDeviceProp_t;
using cudaEvent_t = hipEvent_t;
using cudaStream_t = hipStream_t;

#define cudaDeviceSynchronize hipDeviceSynchronize
#define cudaEventCreate hipEventCreate
#define cudaEventDestroy hipEventDestroy
#define cudaEventElapsedTime hipEventElapsedTime
#define cudaEventRecord hipEventRecord
#define cudaEventSynchronize hipEventSynchronize
#define cudaFree hipFree
#define cudaFreeHost hipHostFree
#define cudaGetDeviceProperties hipGetDeviceProperties
#define cudaGetErrorString hipGetErrorString
#define cudaGetLastError hipGetLastError
#define cudaLaunchCooperativeKernel hipLaunchCooperativeKernel
#define cudaMalloc hipMalloc
#define cudaMallocHost hipHostMalloc
#define cudaMemGetInfo hipMemGetInfo
#define cudaMemcpy hipMemcpy
#define cudaMemcpyAsync hipMemcpyAsync
#define cudaMemcpyDeviceToHost hipMemcpyDeviceToHost
#define cudaMemcpyFromSymbol(dst, symbol, bytes) hipMemcpyFromSymbol((dst), HIP_SYMBOL(symbol), (bytes))
#define cudaMemcpyHostToDevice hipMemcpyHostToDevice
#define cudaMemset hipMemset
#define cudaOccupancyMaxActiveBlocksPerMultiprocessor hipOccupancyMaxActiveBlocksPerMultiprocessor
#define cudaSetDevice hipSetDevice
#define cudaStreamCreate hipStreamCreate
#define cudaStreamDestroy hipStreamDestroy
#define cudaStreamSynchronize hipStreamSynchronize
#define cudaSuccess hipSuccess
#else
#include <cooperative_groups.h>
#include <cuda.h>
#include <cuda_runtime.h>
#endif
