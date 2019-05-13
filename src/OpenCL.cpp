/*******************************************************************************
 * Copyright (c) 2019 Romulo Fernandes Machado
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 ******************************************************************************/


#include "OpenCL.h"
#include <cassert>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#define __ASSERT(X) assert(X)

#if defined(_WIN32) 
// #define DLL_EXPORT __declspec( dllexport )
#include <windows.h>
#define DLL_EXPORT 
#define GETSYMBOL(X,Y) GetProcAddress((X),(Y))
#define LIBRARY_PTR HMODULE
#else
#include <dlfcn.h>
#define DLL_EXPORT
#define GETSYMBOL(X,Y) dlsym((X),(Y))
#define LIBRARY_PTR void*
#endif

typedef void (*f_pfn_notify)(const char *,const void *,size_t,void *);
typedef cl_int (*f_clGetPlatformIDs) (cl_uint,cl_platform_id *,cl_uint *);
typedef cl_int (*f_clGetPlatformInfo) (cl_platform_id,cl_platform_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetDeviceIDs) (cl_platform_id,cl_device_type,cl_uint,cl_device_id *,cl_uint *);
typedef cl_int (*f_clGetDeviceInfo) (cl_device_id,cl_device_info,size_t,void *,size_t *);
typedef cl_int (*f_clCreateSubDevices) (cl_device_id,const cl_device_partition_property *,cl_uint,cl_device_id *,cl_uint *);
typedef cl_int (*f_clRetainDevice) (cl_device_id);
typedef cl_int (*f_clReleaseDevice) (cl_device_id);
typedef cl_context (*f_clCreateContext) (const cl_context_properties *,cl_uint,const cl_device_id *,f_pfn_notify,void *,cl_int *);
typedef cl_context (*f_clCreateContextFromType) (const cl_context_properties *,cl_device_type,f_pfn_notify,void *,cl_int *);
typedef cl_int (*f_clRetainContext) (cl_context);
typedef cl_int (*f_clReleaseContext) (cl_context);
typedef cl_int (*f_clGetContextInfo) (cl_context,cl_context_info,size_t,void *,size_t *);
typedef cl_command_queue (*f_clCreateCommandQueue) (cl_context,cl_device_id,cl_command_queue_properties,cl_int *);
typedef cl_command_queue (*f_clCreateCommandQueueWithProperties) (cl_context,cl_device_id,cl_command_queue_properties,cl_int *);
typedef cl_int (*f_clRetainCommandQueue) (cl_command_queue);
typedef cl_int (*f_clReleaseCommandQueue) (cl_command_queue);
typedef cl_int (*f_clGetCommandQueueInfo) (cl_command_queue,cl_command_queue_info,size_t,void *,size_t *);
typedef cl_mem (*f_clCreateBuffer) (cl_context,cl_mem_flags,size_t,void *,cl_int *);
typedef cl_mem (*f_clCreateSubBuffer) (cl_mem,cl_mem_flags,cl_buffer_create_type,const void *,cl_int *);
typedef cl_mem (*f_clCreateImage) (cl_context,cl_mem_flags,const cl_image_format *,const cl_image_desc *,void *,cl_int *);
typedef cl_mem (*f_clCreatePipe) (cl_context, cl_mem_flags,cl_uint, cl_uint, const cl_pipe_properties *, cl_int *);
typedef cl_int (*f_clRetainMemObject) (cl_mem);
typedef cl_int (*f_clReleaseMemObject) (cl_mem);
typedef cl_int (*f_clGetMemObjectInfo) (cl_mem,cl_mem_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetImageInfo) (cl_mem,cl_image_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetPipeInfo) (cl_mem, cl_pipe_info, size_t, void *, size_t *);
typedef cl_int (*f_clSetMemObjectDestructorCallback) (cl_mem,void (*pfn_notify)( cl_mem memobj,void* user_data),void *);
typedef cl_int (*f_clGetSupportedImageFormats) (cl_context,cl_mem_flags,cl_mem_object_type,cl_uint,cl_image_format *,cl_uint *);
typedef cl_sampler (*f_clCreateSampler) (cl_context,cl_bool,cl_addressing_mode,cl_filter_mode,cl_int *);
typedef cl_int (*f_clRetainSampler) (cl_sampler);
typedef cl_int (*f_clReleaseSampler) (cl_sampler);
typedef cl_int (*f_clGetSamplerInfo) (cl_sampler,cl_sampler_info,size_t,void *,size_t *);
typedef cl_program (*f_clCreateProgramWithSource) (cl_context,cl_uint,const char **,const size_t *,cl_int *);
typedef cl_program (*f_clCreateProgramWithBinary) (cl_context,cl_uint,const cl_device_id *,const size_t *,const unsigned char **,cl_int *,cl_int *);
typedef cl_program (*f_clCreateProgramWithBuiltInKernels) (cl_context,cl_uint,const cl_device_id *,const char *,cl_int *);
typedef cl_int (*f_clRetainProgram) (cl_program);
typedef cl_int (*f_clReleaseProgram) (cl_program);
typedef cl_int (*f_clBuildProgram) (cl_program,cl_uint,const cl_device_id *,const char *,void (*pfn_notify)(cl_program program,void * user_data),void *);
typedef cl_int (*f_clCompileProgram) (cl_program,cl_uint,const cl_device_id *,const char *,cl_uint,const cl_program *,const char **,void (*pfn_notify)(cl_program program,void * user_data),void *);
typedef cl_program (*f_clLinkProgram) (cl_context,cl_uint,const cl_device_id *,const char *,cl_uint,const cl_program *,void (*pfn_notify)(cl_program program,void * user_data),void *,cl_int *);
typedef cl_int (*f_clUnloadPlatformCompiler)(cl_platform_id);
typedef cl_int (*f_clGetProgramInfo) (cl_program,cl_program_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetProgramBuildInfo) (cl_program,cl_device_id,cl_program_build_info,size_t,void *,size_t *);
typedef cl_kernel (*f_clCreateKernel) (cl_program,const char *,cl_int *);
typedef cl_int (*f_clCreateKernelsInProgram) (cl_program,cl_uint,cl_kernel *,cl_uint *);
typedef cl_int (*f_clRetainKernel) (cl_kernel);
typedef cl_int (*f_clReleaseKernel) (cl_kernel);
typedef cl_int (*f_clSetKernelArg) (cl_kernel,cl_uint,size_t,const void *);
typedef cl_int (*f_clGetKernelInfo) (cl_kernel,cl_kernel_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetKernelArgInfo) (cl_kernel,cl_uint,cl_kernel_arg_info,size_t,void *,size_t *);
typedef cl_int (*f_clGetKernelWorkGroupInfo) (cl_kernel,cl_device_id,cl_kernel_work_group_info,size_t,void *,size_t *);
typedef cl_int (*f_clWaitForEvents) (cl_uint,const cl_event *);
typedef cl_int (*f_clGetEventInfo) (cl_event,cl_event_info,size_t,void *,size_t *);
typedef cl_event (*f_clCreateUserEvent) (cl_context,cl_int *);
typedef cl_int (*f_clRetainEvent) (cl_event);
typedef cl_int (*f_clReleaseEvent) (cl_event);
typedef cl_int (*f_clSetUserEventStatus) (cl_event,cl_int);
typedef cl_int (*f_clSetEventCallback) (cl_event,cl_int,void (*pfn_notify)(cl_event,cl_int,void *),void *);
typedef cl_int (*f_clGetEventProfilingInfo) (cl_event,cl_profiling_info,size_t,void *,size_t *);
typedef cl_int (*f_clFlush) (cl_command_queue);
typedef cl_int (*f_clFinish) (cl_command_queue);
typedef cl_int (*f_clEnqueueReadBuffer) (cl_command_queue,cl_mem,cl_bool,size_t,size_t,void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueReadBufferRect) (cl_command_queue,cl_mem,cl_bool,const size_t *,const size_t *,const size_t *,size_t,size_t,size_t,size_t,void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueWriteBuffer) (cl_command_queue,cl_mem,cl_bool,size_t,size_t,const void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueWriteBufferRect) (cl_command_queue,cl_mem,cl_bool,const size_t *,const size_t *,const size_t *,size_t,size_t,size_t,size_t,const void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueFillBuffer) (cl_command_queue,cl_mem,const void *,size_t,size_t,size_t,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueCopyBuffer) (cl_command_queue,cl_mem,cl_mem,size_t,size_t,size_t,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueCopyBufferRect) (cl_command_queue,cl_mem,cl_mem,const size_t *,const size_t *,const size_t *,size_t,size_t,size_t,size_t,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueReadImage) (cl_command_queue,cl_mem,cl_bool,const size_t *,const size_t *,size_t,size_t,void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueWriteImage) (cl_command_queue,cl_mem,cl_bool,const size_t *,const size_t *,size_t,size_t,const void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueFillImage) (cl_command_queue,cl_mem,const void *,const size_t *,const size_t *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueCopyImage) (cl_command_queue,cl_mem,cl_mem,const size_t *,const size_t *,const size_t *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueCopyImageToBuffer) (cl_command_queue,cl_mem,cl_mem,const size_t *,const size_t *,size_t,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueCopyBufferToImage) (cl_command_queue,cl_mem,cl_mem,size_t,const size_t *,const size_t *,cl_uint,const cl_event *,cl_event *);
typedef void * (*f_clEnqueueMapBuffer) (cl_command_queue,cl_mem,cl_bool,cl_map_flags,size_t,size_t,cl_uint,const cl_event *,cl_event *,cl_int *);
typedef void * (*f_clEnqueueMapImage) (cl_command_queue,cl_mem,cl_bool,cl_map_flags,const size_t *,const size_t *,size_t *,size_t *,cl_uint,const cl_event *,cl_event *,cl_int *);
typedef cl_int (*f_clEnqueueUnmapMemObject) (cl_command_queue,cl_mem,void *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueMigrateMemObjects)(cl_command_queue,cl_uint,const cl_mem *,cl_mem_migration_flags,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueNDRangeKernel)(cl_command_queue,cl_kernel,cl_uint,const size_t *,const size_t *,const size_t *,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueTask)(cl_command_queue,cl_kernel,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueNativeKernel)(cl_command_queue,void (*user_func)(void *), void *,size_t,cl_uint,const cl_mem *,const void **,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueMarkerWithWaitList)(cl_command_queue,cl_uint,const cl_event *,cl_event *);
typedef cl_int (*f_clEnqueueBarrierWithWaitList)(cl_command_queue,cl_uint,const cl_event *,cl_event *);
typedef void * (*f_clGetExtensionFunctionAddressForPlatform)(cl_platform_id,const char *);
typedef cl_mem (*f_clCreateImage2D)(cl_context,cl_mem_flags,const cl_image_format *,size_t,size_t,size_t,void *,cl_int *);
typedef cl_mem (*f_clCreateImage3D)(cl_context,cl_mem_flags,const cl_image_format *,size_t,size_t,size_t,size_t,size_t,void *,cl_int *);
typedef cl_int (*f_clEnqueueMarker)(cl_command_queue,cl_event *);
typedef cl_int(*f_clEnqueueWaitForEvents)(cl_command_queue,cl_uint,const cl_event *);
typedef cl_int (*f_clEnqueueBarrier)(cl_command_queue);
typedef cl_int (*f_clUnloadCompiler)(void);
typedef void * (*f_clGetExtensionFunctionAddress)(const char *);

typedef void* (*f_clSVMAlloc) (cl_context, cl_svm_mem_flags, size_t, cl_uint);
typedef void (*f_clSVMFree) (cl_context, void *);
typedef cl_sampler (*f_clCreateSamplerWithProperties) (cl_context, const cl_sampler_properties *, cl_int *);
typedef cl_int (*f_clSetKernelArgSVMPointer) (cl_kernel, cl_uint, const void *);
typedef cl_int (*f_clSetKernelExecInfo) (cl_kernel, cl_kernel_exec_info, size_t, const void *);
typedef cl_int (*f_clEnqueueSVMFree) (cl_command_queue, cl_uint, void *[], void (CL_CALLBACK *)(cl_command_queue, cl_uint, void *[], void *), void *, cl_uint, const cl_event *, cl_event *);
typedef cl_int (*f_clEnqueueSVMMemcpy) (cl_command_queue, cl_bool, void *, const void *, size_t, cl_uint, const cl_event *, cl_event *);
typedef cl_int (*f_clEnqueueSVMMemFill) (cl_command_queue, void *, const void *, size_t, size_t, cl_uint, const cl_event *, cl_event *);
typedef cl_int (*f_clEnqueueSVMMap) (cl_command_queue, cl_bool, cl_map_flags, void *, size_t, cl_uint, const cl_event *, cl_event *);
typedef cl_int (*f_clEnqueueSVMUnmap) (cl_command_queue, void *, cl_uint, const cl_event *, cl_event *);



namespace Implementation {


static LIBRARY_PTR Library; 

static f_pfn_notify Notify;
static f_clGetPlatformIDs GetPlatformIDs;
static f_clGetPlatformInfo GetPlatformInfo;
static f_clGetDeviceIDs GetDeviceIDs;
static f_clGetDeviceInfo GetDeviceInfo;
static f_clCreateSubDevices CreateSubDevices;
static f_clCreateContext CreateContext;
static f_clCreateContextFromType CreateContextFromType;
static f_clRetainDevice RetainDevice;
static f_clReleaseDevice ReleaseDevice;
static f_clRetainContext RetainContext;
static f_clReleaseContext ReleaseContext;
static f_clGetContextInfo GetContextInfo;
static f_clCreateCommandQueue CreateCommandQueue;
static f_clCreateCommandQueueWithProperties CreateCommandQueueWithProperties;
static f_clRetainCommandQueue RetainCommandQueue;
static f_clReleaseCommandQueue ReleaseCommandQueue;
static f_clGetCommandQueueInfo GetCommandQueueInfo;
static f_clCreateBuffer CreateBuffer;
static f_clCreateSubBuffer CreateSubBuffer;
static f_clCreateImage CreateImage;
static f_clCreatePipe CreatePipe;
static f_clRetainMemObject RetainMemObject;
static f_clReleaseMemObject ReleaseMemObject;
static f_clGetMemObjectInfo GetMemObjectInfo;
static f_clGetImageInfo GetImageInfo;
static f_clGetPipeInfo GetPipeInfo;
static f_clSetMemObjectDestructorCallback SetMemObjectDestructorCallback;
static f_clGetSupportedImageFormats GetSupportedImageFormats;
static f_clCreateSampler CreateSampler;
static f_clRetainSampler RetainSampler;
static f_clReleaseSampler ReleaseSampler;
static f_clGetSamplerInfo GetSamplerInfo;
static f_clCreateProgramWithSource CreateProgramWithSource;
static f_clCreateProgramWithBinary CreateProgramWithBinary;
static f_clCreateProgramWithBuiltInKernels CreateProgramWithBuiltInKernels;
static f_clRetainProgram RetainProgram;
static f_clReleaseProgram ReleaseProgram;
static f_clBuildProgram BuildProgram;
static f_clCompileProgram CompileProgram;
static f_clLinkProgram LinkProgram;
static f_clUnloadPlatformCompiler UnloadPlatformCompiler;
static f_clGetProgramInfo GetProgramInfo;
static f_clGetProgramBuildInfo GetProgramBuildInfo;
static f_clCreateKernel CreateKernel;
static f_clCreateKernelsInProgram CreateKernelsInProgram;
static f_clRetainKernel RetainKernel;
static f_clReleaseKernel ReleaseKernel;
static f_clSetKernelArg SetKernelArg;
static f_clGetKernelInfo GetKernelInfo;
static f_clGetKernelArgInfo GetKernelArgInfo;
static f_clGetKernelWorkGroupInfo GetKernelWorkGroupInfo;
static f_clWaitForEvents WaitForEvents;
static f_clGetEventInfo GetEventInfo;
static f_clCreateUserEvent CreateUserEvent;
static f_clRetainEvent RetainEvent;
static f_clReleaseEvent ReleaseEvent;
static f_clSetUserEventStatus SetUserEventStatus;
static f_clSetEventCallback SetEventCallback;
static f_clGetEventProfilingInfo GetEventProfilingInfo;
static f_clFlush Flush;
static f_clFinish Finish;
static f_clEnqueueReadBuffer EnqueueReadBuffer;
static f_clEnqueueReadBufferRect EnqueueReadBufferRect;
static f_clEnqueueWriteBuffer EnqueueWriteBuffer;
static f_clEnqueueWriteBufferRect EnqueueWriteBufferRect;
static f_clEnqueueFillBuffer EnqueueFillBuffer;
static f_clEnqueueCopyBuffer EnqueueCopyBuffer;
static f_clEnqueueCopyBufferRect EnqueueCopyBufferRect;
static f_clEnqueueReadImage EnqueueReadImage;
static f_clEnqueueWriteImage EnqueueWriteImage;
static f_clEnqueueFillImage EnqueueFillImage;
static f_clEnqueueCopyImage EnqueueCopyImage;
static f_clEnqueueCopyImageToBuffer EnqueueCopyImageToBuffer;
static f_clEnqueueCopyBufferToImage EnqueueCopyBufferToImage;
static f_clEnqueueMapBuffer EnqueueMapBuffer;
static f_clEnqueueMapImage EnqueueMapImage;
static f_clEnqueueUnmapMemObject EnqueueUnmapMemObject;
static f_clEnqueueMigrateMemObjects EnqueueMigrateMemObjects;
static f_clEnqueueNDRangeKernel EnqueueNDRangeKernel;
static f_clEnqueueTask EnqueueTask;
static f_clEnqueueNativeKernel EnqueueNativeKernel;
static f_clEnqueueMarkerWithWaitList EnqueueMarkerWithWaitList;
static f_clEnqueueBarrierWithWaitList EnqueueBarrierWithWaitList;
static f_clGetExtensionFunctionAddressForPlatform GetExtensionFunctionAddressForPlatform;
static f_clCreateImage2D CreateImage2D;
static f_clCreateImage3D CreateImage3D;
static f_clEnqueueMarker EnqueueMarker;
static f_clEnqueueWaitForEvents EnqueueWaitForEvents;
static f_clEnqueueBarrier EnqueueBarrier;
static f_clUnloadCompiler UnloadCompiler;
static f_clGetExtensionFunctionAddress GetExtensionFunctionAddress;
static f_clSVMAlloc SVMAlloc;
static f_clSVMFree SVMFree;
static f_clCreateSamplerWithProperties CreateSamplerWithProperties;
static f_clSetKernelArgSVMPointer SetKernelArgSVMPointer;
static f_clSetKernelExecInfo SetKernelExecInfo;
static f_clEnqueueSVMFree EnqueueSVMFree;
static f_clEnqueueSVMMemcpy EnqueueSVMMemcpy;
static f_clEnqueueSVMMemFill EnqueueSVMMemFill;
static f_clEnqueueSVMMap EnqueueSVMMap;
static f_clEnqueueSVMUnmap EnqueueSVMUnmap;


} // namespace Implementation


DLL_EXPORT cl_int CL_API_CALL
clGetPlatformIDs(cl_uint a,
                 cl_platform_id * b,
                 cl_uint * c) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetPlatformIDs(a,b,c);
}

DLL_EXPORT cl_int CL_API_CALL 
clGetPlatformInfo(cl_platform_id a,
                  cl_platform_info b,
                  size_t c,
                  void * d,
                  size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetPlatformInfo(a,b,c,d,e);
}


DLL_EXPORT cl_int CL_API_CALL
clGetDeviceIDs(cl_platform_id a,
               cl_device_type b,
               cl_uint c,
               cl_device_id *d,
               cl_uint *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetDeviceIDs(a,b,c,d,e);
}

DLL_EXPORT cl_int CL_API_CALL
clGetDeviceInfo(cl_device_id a,
                cl_device_info b,
                size_t c,
                void *d,
                size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetDeviceInfo(a,b,c,d,e);
}
    
DLL_EXPORT cl_int CL_API_CALL
clCreateSubDevices(cl_device_id a,
                   const cl_device_partition_property *b,
                   cl_uint c,
                   cl_device_id *d,
                   cl_uint *e) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::CreateSubDevices(a,b,c,d,e);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainDevice(cl_device_id a) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::RetainDevice(a);
}
    
DLL_EXPORT cl_int CL_API_CALL
clReleaseDevice(cl_device_id a) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::ReleaseDevice(a);
}
    

DLL_EXPORT cl_context CL_API_CALL
clCreateContext(const cl_context_properties *a,
                cl_uint b,
                const cl_device_id *c,
                void (CL_CALLBACK *d)(const char *,const void *,size_t,void *),
                void *e,
                cl_int *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateContext(a,b,c,d,e,f);
}

DLL_EXPORT cl_context CL_API_CALL
clCreateContextFromType(const cl_context_properties *a,
                        cl_device_type b,
                        void (CL_CALLBACK *c)(const char *,const void *,size_t,void *),
                        void *d,
                        cl_int *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateContextFromType(a,b,c,d,e);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainContext(cl_context a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainContext(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseContext(cl_context a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseContext(a);
}

DLL_EXPORT cl_int CL_API_CALL
clGetContextInfo(cl_context a,
                 cl_context_info b,
                 size_t c,
                 void *d,
                 size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetContextInfo(a,b,c,d,e);
}


DLL_EXPORT cl_command_queue CL_API_CALL
clCreateCommandQueueWithProperties(cl_context a,
                                   cl_device_id b,
                                   const cl_queue_properties c,
                                   cl_int *d) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::CreateCommandQueueWithProperties(a, b, c, d);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainCommandQueue(cl_command_queue a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainCommandQueue(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseCommandQueue(cl_command_queue a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseCommandQueue(a);
}

DLL_EXPORT cl_int CL_API_CALL
clGetCommandQueueInfo(cl_command_queue a,
                      cl_command_queue_info b,
                      size_t c,
                      void *d,
                      size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetCommandQueueInfo(a, b, c, d, e);
}


DLL_EXPORT cl_mem CL_API_CALL
clCreateBuffer(cl_context a,
               cl_mem_flags b,
               size_t c,
               void *d,
               cl_int *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateBuffer(a, b, c, d, e);
}

DLL_EXPORT cl_mem CL_API_CALL
clCreateSubBuffer(cl_mem a,
                  cl_mem_flags b,
                  cl_buffer_create_type c,
                  const void *d,
                  cl_int *e) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::CreateSubBuffer(a, b, c, d, e);
}

DLL_EXPORT cl_mem CL_API_CALL
clCreateImage(cl_context a,
              cl_mem_flags b,
              const cl_image_format *c,
              const cl_image_desc *d,
              void *e,
              cl_int *f) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::CreateImage(a, b, c, d, e, f);
}
                        
DLL_EXPORT cl_mem CL_API_CALL
clCreatePipe(cl_context a,
             cl_mem_flags b,
             cl_uint c,
             cl_uint d,
             const cl_pipe_properties *e,
             cl_int *f) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::CreatePipe(a, b, c, d, e, f);
}
    
DLL_EXPORT cl_int CL_API_CALL
clRetainMemObject(cl_mem a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainMemObject(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseMemObject(cl_mem a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseMemObject(a);
}

DLL_EXPORT cl_int CL_API_CALL
clGetSupportedImageFormats(cl_context a,
                           cl_mem_flags b,
                           cl_mem_object_type c,
                           cl_uint d,
                           cl_image_format *e,
                           cl_uint *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetSupportedImageFormats(a, b, c, d, e, f);
}
                                    
DLL_EXPORT cl_int CL_API_CALL
clGetMemObjectInfo(cl_mem a,
                   cl_mem_info b,
                   size_t c,
                   void *d,
                   size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetMemObjectInfo(a, b, c, d, e);
}

DLL_EXPORT cl_int CL_API_CALL
clGetImageInfo(cl_mem a,
               cl_image_info b,
               size_t c,
               void *d,
               size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetImageInfo(a, b, c, d, e);
}
    
DLL_EXPORT cl_int CL_API_CALL
clGetPipeInfo(cl_mem a,
              cl_pipe_info b,
              size_t c,
              void *d,
              size_t *e) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::GetPipeInfo(a, b, c, d, e);
}
    

DLL_EXPORT cl_int CL_API_CALL
clSetMemObjectDestructorCallback(cl_mem a,
                                 void (CL_CALLBACK *b)( cl_mem,void*),
                                 void *c) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::SetMemObjectDestructorCallback(a, b, c);
}


DLL_EXPORT void * CL_API_CALL
clSVMAlloc(cl_context a,
           cl_svm_mem_flags b,
           size_t c,
           cl_uint d) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::SVMAlloc(a, b, c, d);
}

DLL_EXPORT void CL_API_CALL
clSVMFree(cl_context a,
          void *b) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::SVMFree(a, b);
}
    

DLL_EXPORT cl_sampler CL_API_CALL
clCreateSamplerWithProperties(cl_context a,
                              const cl_sampler_properties *b,
                              cl_int *c) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::CreateSamplerWithProperties(a, b, c);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainSampler(cl_sampler a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainSampler(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseSampler(cl_sampler a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseSampler(a);
}

DLL_EXPORT cl_int CL_API_CALL
clGetSamplerInfo(cl_sampler a,
                 cl_sampler_info b,
                 size_t c,
                 void *d,
                 size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetSamplerInfo(a, b, c, d, e);
}
                            

DLL_EXPORT cl_program CL_API_CALL
clCreateProgramWithSource(cl_context a,
                          cl_uint b,
                          const char **c,
                          const size_t *d,
                          cl_int *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateProgramWithSource(a, b, c, d, e);
}

DLL_EXPORT cl_program CL_API_CALL
clCreateProgramWithBinary(cl_context a,
                          cl_uint b,
                          const cl_device_id *c,
                          const size_t *d,
                          const unsigned char **e,
                          cl_int *f,
                          cl_int *g) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateProgramWithBinary(a, b, c, d, e, f, g);
}

DLL_EXPORT cl_program CL_API_CALL
clCreateProgramWithBuiltInKernels(cl_context a,
                                  cl_uint b,
                                  const cl_device_id *c,
                                  const char *d,
                                  cl_int *e) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::CreateProgramWithBuiltInKernels(a, b, c, d, e);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainProgram(cl_program a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainProgram(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseProgram(cl_program a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseProgram(a);
}

DLL_EXPORT cl_int CL_API_CALL
clBuildProgram(cl_program a,
               cl_uint b,
               const cl_device_id *c,
               const char *d,
               void (CL_CALLBACK *e)(cl_program,void *),
               void *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::BuildProgram(a, b, c, d, e, f);
}

DLL_EXPORT cl_int CL_API_CALL
clCompileProgram(cl_program a,
                 cl_uint b,
                 const cl_device_id *c,
                 const char *d,
                 cl_uint e,
                 const cl_program *f,
                 const char **g,
                 void (CL_CALLBACK *h)(cl_program,void *),
                 void *i) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::CompileProgram(a, b, c, d, e, f, g, h, i);
}

DLL_EXPORT cl_program CL_API_CALL
clLinkProgram(cl_context a,
              cl_uint b,
              const cl_device_id *c,
              const char *d,
              cl_uint e,
              const cl_program *f,
              void (CL_CALLBACK *g)(cl_program,void *),
              void *h,
              cl_int *i) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::LinkProgram(a, b, c, d, e, f, g, h, i);
}


DLL_EXPORT cl_int CL_API_CALL
clUnloadPlatformCompiler(cl_platform_id a) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::UnloadPlatformCompiler(a);
}

DLL_EXPORT cl_int CL_API_CALL
clGetProgramInfo(cl_program a,
                 cl_program_info b,
                 size_t c,
                 void *d,
                 size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetProgramInfo(a, b, c, d, e);
}

DLL_EXPORT cl_int CL_API_CALL
clGetProgramBuildInfo(cl_program a,
                      cl_device_id b,
                      cl_program_build_info c,
                      size_t d,
                      void *e,
                      size_t *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetProgramBuildInfo(a, b, c, d, e, f);
}
                            

DLL_EXPORT cl_kernel CL_API_CALL
clCreateKernel(cl_program a,
               const char *b,
               cl_int *c) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateKernel(a, b, c);
}

DLL_EXPORT cl_int CL_API_CALL
clCreateKernelsInProgram(cl_program a,
                         cl_uint b,
                         cl_kernel *c,
                         cl_uint *d) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::CreateKernelsInProgram(a, b, c, d);
}

DLL_EXPORT cl_int CL_API_CALL
clRetainKernel(cl_kernel a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainKernel(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseKernel(cl_kernel a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseKernel(a);
}

DLL_EXPORT cl_int CL_API_CALL
clSetKernelArg(cl_kernel a,
               cl_uint b,
               size_t c,
               const void *d) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::SetKernelArg(a, b, c, d);
}

DLL_EXPORT cl_int CL_API_CALL
clSetKernelArgSVMPointer(cl_kernel a,
                         cl_uint b,
                         const void *c) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::SetKernelArgSVMPointer(a, b, c);
}

DLL_EXPORT cl_int CL_API_CALL
clSetKernelExecInfo(cl_kernel a,
                    cl_kernel_exec_info b,
                    size_t c,
                    const void *d) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::SetKernelExecInfo(a, b, c, d);
}
    
DLL_EXPORT cl_int CL_API_CALL
clGetKernelInfo(cl_kernel a,
                cl_kernel_info b,
                size_t c,
                void *d,
                size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetKernelInfo(a, b, c, d, e);
}

DLL_EXPORT cl_int CL_API_CALL
clGetKernelArgInfo(cl_kernel a,
                   cl_uint b,
                   cl_kernel_arg_info c,
                   size_t d,
                   void *e,
                   size_t *f) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::GetKernelArgInfo(a, b, c, d, e, f);
}

DLL_EXPORT cl_int CL_API_CALL
clGetKernelWorkGroupInfo(cl_kernel a,
                         cl_device_id b,
                         cl_kernel_work_group_info c,
                         size_t d,
                         void *e,
                         size_t *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetKernelWorkGroupInfo(a, b, c, d, e, f);
}


DLL_EXPORT cl_int CL_API_CALL
clWaitForEvents(cl_uint a, const cl_event *b) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::WaitForEvents(a, b);
}

DLL_EXPORT cl_int CL_API_CALL
clGetEventInfo(cl_event a,
               cl_event_info b,
               size_t c,
               void *d,
               size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetEventInfo(a, b, c, d, e);
}
                            
DLL_EXPORT cl_event CL_API_CALL
clCreateUserEvent(cl_context a, cl_int *b) CL_API_SUFFIX__VERSION_1_1 {               

  return Implementation::CreateUserEvent(a, b);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clRetainEvent(cl_event a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::RetainEvent(a);
}

DLL_EXPORT cl_int CL_API_CALL
clReleaseEvent(cl_event a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::ReleaseEvent(a);
}

DLL_EXPORT cl_int CL_API_CALL
clSetUserEventStatus(cl_event a, cl_int b) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::SetUserEventStatus(a, b);
}
                     
DLL_EXPORT cl_int CL_API_CALL
clSetEventCallback(cl_event a,
                   cl_int b,
                   void (CL_CALLBACK *c)(cl_event,cl_int,void *),
                   void *d) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::SetEventCallback(a, b, c, d);
}


DLL_EXPORT cl_int CL_API_CALL
clGetEventProfilingInfo(cl_event a,
                        cl_profiling_info b,
                        size_t c,
                        void *d,
                        size_t *e) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::GetEventProfilingInfo(a, b, c, d, e);
}
                                

DLL_EXPORT cl_int CL_API_CALL
clFlush(cl_command_queue a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::Flush(a);
}

DLL_EXPORT cl_int CL_API_CALL
clFinish(cl_command_queue a) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::Finish(a);
}


DLL_EXPORT cl_int CL_API_CALL
clEnqueueReadBuffer(cl_command_queue a,
                    cl_mem b,
                    cl_bool c,
                    size_t d,
                    size_t e,
                    void *f,
                    cl_uint g,
                    const cl_event *h,
                    cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueReadBuffer(a, b, c, d, e, f, g, h, i);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueReadBufferRect(cl_command_queue a,
                        cl_mem b,
                        cl_bool c,
                        const size_t *d,
                        const size_t *e,
                        const size_t *f,
                        size_t g,
                        size_t h,
                        size_t i,
                        size_t j,                       
                        void *k,
                        cl_uint l,
                        const cl_event *m,
                        cl_event *n) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::EnqueueReadBufferRect(a, b, c, d, e, f, g, 
    h, i, j, k, l, m, n);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueWriteBuffer(cl_command_queue a,
                     cl_mem b,
                     cl_bool c,
                     size_t d,
                     size_t e,
                     const void *f,
                     cl_uint g,
                     const cl_event *h,
                     cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueWriteBuffer(a, b, c, d, e, f, g, h, i);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueWriteBufferRect(cl_command_queue a,
                         cl_mem b,
                         cl_bool c,
                         const size_t *d,
                         const size_t *e,
                         const size_t *f,
                         size_t g,
                         size_t h,
                         size_t i,
                         size_t j,                       
                         const void *k,
                         cl_uint l,
                         const cl_event *m,
                         cl_event *n) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::EnqueueWriteBufferRect(a, b, c, d, e, f, g, h, i, 
    j, k, l, m, n);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueFillBuffer(cl_command_queue a,
                    cl_mem b,
                    const void *c,
                    size_t d,
                    size_t e,
                    size_t f,
                    cl_uint g,
                    const cl_event *h,
                    cl_event *i) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::EnqueueFillBuffer(a, b, c, d, e, f, g, h, i);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueCopyBuffer(cl_command_queue a,
                    cl_mem b,
                    cl_mem c,
                    size_t d,
                    size_t e,
                    size_t f,
                    cl_uint g,
                    const cl_event *h,
                    cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueCopyBuffer(a, b, c, d, e, f, g, h, i);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueCopyBufferRect(cl_command_queue a,
                        cl_mem b,
                        cl_mem c,
                        const size_t *d,
                        const size_t *e,
                        const size_t *f,
                        size_t g,
                        size_t h,
                        size_t i,
                        size_t j,
                        cl_uint k,
                        const cl_event *l,
                        cl_event *m) CL_API_SUFFIX__VERSION_1_1 {

  return Implementation::EnqueueCopyBufferRect(a, b, c, d, e, f, g, h, i, j, k, l, m);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueReadImage(cl_command_queue a,
                   cl_mem b,
                   cl_bool c,
                   const size_t *d,
                   const size_t *e,
                   size_t f,
                   size_t g,
                   void *h,
                   cl_uint i,
                   const cl_event *j,
                   cl_event *k) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueReadImage(a, b, c, d, e, f, g, h, i, j, k);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueWriteImage(cl_command_queue a,
                    cl_mem b,
                    cl_bool c,
                    const size_t *d,
                    const size_t *e,
                    size_t f,
                    size_t g,
                    const void *h,
                    cl_uint i,
                    const cl_event *j,
                    cl_event *k) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueWriteImage(a, b, c, d, e, f, g, h, i, j, k);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueFillImage(cl_command_queue a,
                   cl_mem b,
                   const void *c,
                   const size_t *d,
                   const size_t *e,
                   cl_uint f,
                   const cl_event *g,
                   cl_event *h) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::EnqueueFillImage(a, b, c, d, e, f, g, h);
}
                            
DLL_EXPORT cl_int CL_API_CALL
clEnqueueCopyImage(cl_command_queue a,
                   cl_mem b,
                   cl_mem c,
                   const size_t *d,
                   const size_t *e,
                   const size_t *f,
                   cl_uint g,
                   const cl_event *h,
                   cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueCopyImage(a, b, c, d, e, f, g, h, i);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueCopyImageToBuffer(cl_command_queue a,
                           cl_mem b,
                           cl_mem c,
                           const size_t *d,
                           const size_t *e,
                           size_t f,
                           cl_uint g,
                           const cl_event *h,
                           cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueCopyImageToBuffer(a, b, c, d, e, f, g, h, i);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueCopyBufferToImage(cl_command_queue a,
                           cl_mem b,
                           cl_mem c,
                           size_t d,
                           const size_t *e,
                           const size_t *f,
                           cl_uint g,
                           const cl_event *h,
                           cl_event *i) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueCopyBufferToImage(a, b, c, d, e, f, g, h, i);
}

DLL_EXPORT void * CL_API_CALL
clEnqueueMapBuffer(cl_command_queue a,
                   cl_mem b,
                   cl_bool c,
                   cl_map_flags d,
                   size_t e,
                   size_t f,
                   cl_uint g,
                   const cl_event *h,
                   cl_event *i,
                   cl_int *j) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueMapBuffer(a, b, c, d, e, f, g, h, i, j);
}

DLL_EXPORT void * CL_API_CALL
clEnqueueMapImage(cl_command_queue a,
                  cl_mem b,
                  cl_bool c,
                  cl_map_flags d,
                  const size_t *e,
                  const size_t *f,
                  size_t *g,
                  size_t *h,
                  cl_uint i,
                  const cl_event *j,
                  cl_event *k,
                  cl_int *l) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueMapImage(a, b, c, d, e, f, g, h, i, j, k, l);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueUnmapMemObject(cl_command_queue a,
                        cl_mem b,
                        void *c,
                        cl_uint d,
                        const cl_event *e,
                        cl_event *f) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueUnmapMemObject(a, b, c, d, e, f);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueMigrateMemObjects(cl_command_queue a,
                           cl_uint b,
                           const cl_mem *c,
                           cl_mem_migration_flags d,
                           cl_uint e,
                           const cl_event *f,
                           cl_event *g) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::EnqueueMigrateMemObjects(a, b, c, d, e, f, g);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueNDRangeKernel(cl_command_queue a,
                       cl_kernel b,
                       cl_uint c,
                       const size_t *d,
                       const size_t *e,
                       const size_t *f,
                       cl_uint g,
                       const cl_event *i,
                       cl_event *j) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueNDRangeKernel(a, b, c, d, e, f, g, i, j);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueNativeKernel(cl_command_queue a,
					            void (CL_CALLBACK *b)(void *),
                      void *c,
                      size_t d,
                      cl_uint e,
                      const cl_mem *f,
                      const void **g,
                      cl_uint h,
                      const cl_event *i,
                      cl_event *j) CL_API_SUFFIX__VERSION_1_0 {

  return Implementation::EnqueueNativeKernel(a, b, c, d, e, f, g, h, i, j);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueMarkerWithWaitList(cl_command_queue a,
                            cl_uint b,
                            const cl_event *c,
                            cl_event *d) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::EnqueueMarkerWithWaitList(a, b, c, d);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueBarrierWithWaitList(cl_command_queue a,
                             cl_uint b,
                             const cl_event *c,
                             cl_event *d) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::EnqueueBarrierWithWaitList(a, b, c, d);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueSVMFree(cl_command_queue a,
                 cl_uint b,
                 void *c[],
                 void (CL_CALLBACK *d)(cl_command_queue, cl_uint, void *[], void *),
                 void *e,
                 cl_uint f,
                 const cl_event *g,
                 cl_event *h) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::EnqueueSVMFree(a, b, c, d, e, f, g, h);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueSVMMemcpy(cl_command_queue a,
                   cl_bool b,
                   void *c,
                   const void *d,
                   size_t e,
                   cl_uint f,
                   const cl_event *g,
                   cl_event *h) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::EnqueueSVMMemcpy(a, b, c, d, e, f, g, h);
}

DLL_EXPORT cl_int CL_API_CALL
clEnqueueSVMMemFill(cl_command_queue a,
                    void *b,
                    const void *c,
                    size_t d,
                    size_t e,
                    cl_uint f,
                    const cl_event *g,
                    cl_event *h) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::EnqueueSVMMemFill(a, b, c, d, e, f, g, h);
}
    
DLL_EXPORT cl_int CL_API_CALL
clEnqueueSVMMap(cl_command_queue a,
                cl_bool b,
                cl_map_flags c,
                void *d,
                size_t e,
                cl_uint f,
                const cl_event *g,
                cl_event *h) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::EnqueueSVMMap(a, b, c, d, e, f, g, h);
}
    
DLL_EXPORT cl_int CL_API_CALL
clEnqueueSVMUnmap(cl_command_queue a,
                  void *b,
                  cl_uint c,
                  const cl_event *d,
                  cl_event *e) CL_API_SUFFIX__VERSION_2_0 {

  return Implementation::EnqueueSVMUnmap(a, b, c, d, e);
}
    

/* Extension function access
 *
 * Returns the extension function address for the given function name,
 * or NULL if a valid function can not be found.  The client must
 * check to make sure the address is not NULL,before using or 
 * calling the returned function address.
 */
DLL_EXPORT void * CL_API_CALL 
clGetExtensionFunctionAddressForPlatform(cl_platform_id a,
                                         const char *b) CL_API_SUFFIX__VERSION_1_2 {

  return Implementation::GetExtensionFunctionAddressForPlatform(a, b);
}
    

/* Deprecated OpenCL 1.1 APIs */
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
clCreateImage2D(cl_context a,
                cl_mem_flags b,
                const cl_image_format *c,
                size_t d,
                size_t e,
                size_t f,
                void *g,
                cl_int *i) {

  return Implementation::CreateImage2D(a, b, c, d, e, f, g, i);
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
clCreateImage3D(cl_context a,
                cl_mem_flags b,
                const cl_image_format *c,
                size_t d,
                size_t e,
                size_t f,
                size_t g,
                size_t h,
                void *i,
                cl_int *j) {

  return Implementation::CreateImage3D(a, b, c, d, e, f, g, h, i, j);
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clEnqueueMarker(cl_command_queue a,
                cl_event *b) {

  return Implementation::EnqueueMarker(a, b);
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clEnqueueWaitForEvents(cl_command_queue a,
                        cl_uint b,
                        const cl_event *c) {

  return Implementation::EnqueueWaitForEvents(a, b, c);
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clEnqueueBarrier(cl_command_queue a) {

  return Implementation::EnqueueBarrier(a);
}

DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clUnloadCompiler(void) {

  return Implementation::UnloadCompiler();
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_1_1_DEPRECATED void * CL_API_CALL
clGetExtensionFunctionAddress(const char *a) {

  return Implementation::GetExtensionFunctionAddress(a);
}
    
/* Deprecated OpenCL 2.0 APIs */
DLL_EXPORT CL_EXT_PREFIX__VERSION_2_0_DEPRECATED cl_command_queue CL_API_CALL
clCreateCommandQueue(cl_context a,
                     cl_device_id b,
                     cl_command_queue_properties c,
                     cl_int *d) {

  return Implementation::CreateCommandQueue(a, b, c, d);
}
    
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_2_0_DEPRECATED cl_sampler CL_API_CALL
clCreateSampler(cl_context a,
                cl_bool b,
                cl_addressing_mode c,
                cl_filter_mode d,
                cl_int *e) {

  return Implementation::CreateSampler(a, b, c, d, e);
}
    
DLL_EXPORT CL_EXT_PREFIX__VERSION_2_0_DEPRECATED cl_int CL_API_CALL
clEnqueueTask(cl_command_queue a,
              cl_kernel b,
              cl_uint c,
              const cl_event *d,
              cl_event *e) {

  return Implementation::EnqueueTask(a, b, c, d, e);
}

struct DLL_EXPORT ComputeDevice {
  cl_platform_id platformID;
  std::string platformName;

  cl_device_id deviceID;
  std::string deviceName;

  /**
     \brief Expõe a recuperação da plataforma para o Java.
     (DISPONÍVEL PARA O JAVA)
  */
  const std::string& getPlatform() const {
    return platformName;
  }

  /**
     \brief Expõe a recuperação do dispositvo para o Java.
     (DISPONÍVEL PARA O JAVA)
  */
  const std::string& getDevice() const {
    return deviceName;
  }

  /**
     \brief Retorna o nome composto do dispositivo.
     (DISPONÍVEL PARA O JAVA).
  */
  std::string getCompositeName() const {
    return deviceName + " - " + platformName;
  }
};

std::vector< ComputeDevice > getDevicesInformation() {
  std::vector< ComputeDevice > resultDevices;

  cl_uint numPlatformsAvailable = 0;
  clGetPlatformIDs( 0,nullptr,&numPlatformsAvailable );

  std::vector< cl_platform_id > platforms( numPlatformsAvailable,nullptr );
  clGetPlatformIDs( static_cast< cl_uint >( platforms.size() ),
    platforms.data(),&numPlatformsAvailable );

  for ( size_t i = 0; i < numPlatformsAvailable; ++i ) {
    char tmpName[ 4096 ] = {};
    size_t nameLength = 0;

    clGetPlatformInfo( platforms[ i ],CL_PLATFORM_NAME,4096,tmpName,
      &nameLength );

    cl_uint numDevicesAvailable = 0;
    clGetDeviceIDs( platforms[ i ],CL_DEVICE_TYPE_ALL,0,nullptr,
      &numDevicesAvailable );

    const std::string platformName( tmpName );
    std::vector< cl_device_id > devices( numDevicesAvailable,nullptr );

    clGetDeviceIDs( platforms[ i ],CL_DEVICE_TYPE_ALL,
      static_cast< cl_uint >( devices.size() ), devices.data(),
      &numDevicesAvailable );

    __ASSERT( numDevicesAvailable <= devices.size() );
    for ( size_t j = 0; j < numDevicesAvailable; ++j ) {
      cl_device_type tmpDeviceType;
      char tmpDeviceName[ 4096 ] = {};
      size_t deviceNameLength = 0;

      clGetDeviceInfo( devices[ j ],CL_DEVICE_NAME,4096,tmpDeviceName,
        &deviceNameLength );

      clGetDeviceInfo( devices[ j ],CL_DEVICE_TYPE,sizeof( cl_device_type ),
        &tmpDeviceType,nullptr );

      const bool isGPU = tmpDeviceType == CL_DEVICE_TYPE_GPU;
      const bool isAccelerator = tmpDeviceType == CL_DEVICE_TYPE_ACCELERATOR;
      if ( isGPU || isAccelerator ) {
        const std::string deviceName( tmpDeviceName );
        resultDevices.push_back( { platforms[ i ],platformName,
          devices[ j ],deviceName } );
      }
    }
  }

  return resultDevices;
}

#define throwException(X) \
  do {\
    std::stringstream _str;\
    _str << X;\
    std::cout << _str.str() << std::endl;\
    std::exit(0);\
  } while (false)

#define RESOLVE(_X_) \
  do {\
    Implementation::_X_ = (f_cl ## _X_) GETSYMBOL(Implementation::Library, "cl" # _X_);\
    if ( !( Implementation::_X_ ) )\
      throwException( "Failed to load symbol cl" # _X_ ":" << GetLastErrorAsString() );\
  } while (false)

#ifdef _WIN32
std::string GetLastErrorAsString() {
  //Get the error message,if any.
  DWORD errorMessageID = ::GetLastError();
  if(errorMessageID == 0)
    return std::string(); //No error message has been recorded

  LPSTR messageBuffer = nullptr;
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,errorMessageID,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPSTR)&messageBuffer,0,NULL);

  std::string message(messageBuffer,size);

  //Free the buffer.
  LocalFree(messageBuffer);

  return message;
}
#else
std::string GetLastErrorAsString() {
  return std::string( dlerror() );
}
#endif

void initializeOpenCLStub() {
#ifdef _WIN32
  const std::string libraryPath( "OpenCL.dll" );
  Implementation::Library = LoadLibrary( libraryPath.c_str() );
  if ( !Implementation::Library ) {
    throwException( "Failed to open library: " << GetLastErrorAsString() );
  }
#else
  const std::string libraryPaths[] = {
    "libOpenCL.so",
    "libOpenCL.so.1"
  };

  for (const std::string& libraryPath : libraryPaths) {
    Implementation::Library = dlopen( libraryPath.c_str(), RTLD_LAZY | RTLD_GLOBAL );
    if ( Implementation::Library )
      break;
  }

  if ( !Implementation::Library ) {
    throwException( "Failed to open library: " << GetLastErrorAsString() );
  }
#endif

  RESOLVE(GetPlatformIDs);
  RESOLVE(GetPlatformInfo);
  RESOLVE(GetDeviceIDs);
  RESOLVE(GetDeviceInfo);
  RESOLVE(CreateSubDevices);
  RESOLVE(CreateContext);
  RESOLVE(CreateContextFromType);
  RESOLVE(RetainDevice);
  RESOLVE(ReleaseDevice);
  RESOLVE(RetainContext);
  RESOLVE(ReleaseContext);
  RESOLVE(GetContextInfo);
  RESOLVE(CreateCommandQueue);
  RESOLVE(CreateCommandQueueWithProperties);
  RESOLVE(RetainCommandQueue);
  RESOLVE(ReleaseCommandQueue);
  RESOLVE(GetCommandQueueInfo);
  RESOLVE(CreateBuffer);
  RESOLVE(CreateSubBuffer);
  RESOLVE(CreateImage);
  RESOLVE(CreatePipe);
  RESOLVE(RetainMemObject);
  RESOLVE(ReleaseMemObject);
  RESOLVE(GetMemObjectInfo);
  RESOLVE(GetImageInfo);
  RESOLVE(GetPipeInfo);
  RESOLVE(SetMemObjectDestructorCallback);
  RESOLVE(GetSupportedImageFormats);
  RESOLVE(CreateSampler);
  RESOLVE(RetainSampler);
  RESOLVE(ReleaseSampler);
  RESOLVE(GetSamplerInfo);
  RESOLVE(CreateProgramWithSource);
  RESOLVE(CreateProgramWithBinary);
  RESOLVE(CreateProgramWithBuiltInKernels);
  RESOLVE(RetainProgram);
  RESOLVE(ReleaseProgram);
  RESOLVE(BuildProgram);
  RESOLVE(CompileProgram);
  RESOLVE(LinkProgram);
  RESOLVE(UnloadPlatformCompiler);
  RESOLVE(GetProgramInfo);
  RESOLVE(GetProgramBuildInfo);
  RESOLVE(CreateKernel);
  RESOLVE(CreateKernelsInProgram);
  RESOLVE(RetainKernel);
  RESOLVE(ReleaseKernel);
  RESOLVE(SetKernelArg);
  RESOLVE(GetKernelInfo);
  RESOLVE(GetKernelArgInfo);
  RESOLVE(GetKernelWorkGroupInfo);
  RESOLVE(WaitForEvents);
  RESOLVE(GetEventInfo);
  RESOLVE(CreateUserEvent);
  RESOLVE(RetainEvent);
  RESOLVE(ReleaseEvent);
  RESOLVE(SetUserEventStatus);
  RESOLVE(SetEventCallback);
  RESOLVE(GetEventProfilingInfo);
  RESOLVE(Flush);
  RESOLVE(Finish);
  RESOLVE(EnqueueReadBuffer);
  RESOLVE(EnqueueReadBufferRect);
  RESOLVE(EnqueueWriteBuffer);
  RESOLVE(EnqueueWriteBufferRect);
  RESOLVE(EnqueueFillBuffer);
  RESOLVE(EnqueueCopyBuffer);
  RESOLVE(EnqueueCopyBufferRect);
  RESOLVE(EnqueueReadImage);
  RESOLVE(EnqueueWriteImage);
  RESOLVE(EnqueueFillImage);
  RESOLVE(EnqueueCopyImage);
  RESOLVE(EnqueueCopyImageToBuffer);
  RESOLVE(EnqueueCopyBufferToImage);
  RESOLVE(EnqueueMapBuffer);
  RESOLVE(EnqueueMapImage);
  RESOLVE(EnqueueUnmapMemObject);
  RESOLVE(EnqueueMigrateMemObjects);
  RESOLVE(EnqueueNDRangeKernel);
  RESOLVE(EnqueueTask);
  RESOLVE(EnqueueNativeKernel);
  RESOLVE(EnqueueMarkerWithWaitList);
  RESOLVE(EnqueueBarrierWithWaitList);
  RESOLVE(GetExtensionFunctionAddressForPlatform);
  RESOLVE(CreateImage2D);
  RESOLVE(CreateImage3D);
  RESOLVE(EnqueueMarker);
  RESOLVE(EnqueueWaitForEvents);
  RESOLVE(EnqueueBarrier);
  RESOLVE(UnloadCompiler);
  RESOLVE(GetExtensionFunctionAddress);
  RESOLVE(SVMAlloc);
  RESOLVE(SVMFree);
  RESOLVE(CreateSamplerWithProperties);
  RESOLVE(SetKernelArgSVMPointer);
  RESOLVE(SetKernelExecInfo);
  RESOLVE(EnqueueSVMFree);
  RESOLVE(EnqueueSVMMemcpy);
  RESOLVE(EnqueueSVMMemFill);
  RESOLVE(EnqueueSVMMap);
  RESOLVE(EnqueueSVMUnmap);
}

int main() {
  initializeOpenCLStub();
  auto devices = getDevicesInformation();

  std::cout << "Loaded devices:" << std::endl;
  for (auto& device : devices) {
    std::cout << device.platformName << "," << device.deviceName << std::endl;
  }
}


