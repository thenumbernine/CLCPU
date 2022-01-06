// honestly if my names are matching then all I need to do is use cl.h
// but ... should my names match? I think they shouldn't -- of anything for safety
#include "CLCPU/Dispatch.h"
#include "CLCPU/Platform.h"
#include "CLCPU/Device.h"
#include "CLCPU/Context.h"
#include "CLCPU/CommandQueue.h"
#include "CLCPU/Memory.h"
#include "CLCPU/Sampler.h"

cl_icd_dispatch dispatchTable = {
	/* OpenCL 1.0 */
	//Platform.h
	clGetPlatformIDs,
	clGetPlatformInfo,
	//Device.h
	clGetDeviceIDs,
	clGetDeviceInfo,
	//Context.h
	clCreateContext,
	clCreateContextFromType,
	clRetainContext,
	clReleaseContext,
	clGetContextInfo,
	//CommandQueue.h
	clCreateCommandQueue,
	clRetainCommandQueue,
	clReleaseCommandQueue,
	clGetCommandQueueInfo,
	clSetCommandQueueProperty,
	
	clCreateBuffer,				//Memory.h
	clCreateImage2D,			//Sampler.h
	clCreateImage3D,			//Sampler.h
	clRetainMemObject,			//Memory.h
	clReleaseMemObject,			//Memory.h
	clGetSupportedImageFormats,	//Sampler.h
	clGetMemObjectInfo,			//Memory.h
	
	//Sampler.h
	clGetImageInfo,
	clCreateSampler,
	clRetainSampler,
	clReleaseSampler,
	clGetSamplerInfo,

	//Program.h
	clCreateProgramWithSource,
	clCreateProgramWithBinary,
	clRetainProgram,
	clReleaseProgram,
	clBuildProgram,
	clUnloadCompiler,
	clGetProgramInfo,
	clGetProgramBuildInfo,

	//Kernel.h
	clCreateKernel,
	clCreateKernelsInProgram,
	clRetainKernel,
	clReleaseKernel,
	clSetKernelArg,
	clGetKernelInfo,
	clGetKernelWorkGroupInfo,

#if 0
	cl_api_clWaitForEvents clWaitForEvents;
	cl_api_clGetEventInfo clGetEventInfo;
	cl_api_clRetainEvent clRetainEvent;
	cl_api_clReleaseEvent clReleaseEvent;
	cl_api_clGetEventProfilingInfo clGetEventProfilingInfo;
	cl_api_clFlush clFlush;
	cl_api_clFinish clFinish;
	cl_api_clEnqueueReadBuffer clEnqueueReadBuffer;
	cl_api_clEnqueueWriteBuffer clEnqueueWriteBuffer;
	cl_api_clEnqueueCopyBuffer clEnqueueCopyBuffer;
	cl_api_clEnqueueReadImage clEnqueueReadImage;
	cl_api_clEnqueueWriteImage clEnqueueWriteImage;
	cl_api_clEnqueueCopyImage clEnqueueCopyImage;
	cl_api_clEnqueueCopyImageToBuffer clEnqueueCopyImageToBuffer;
	cl_api_clEnqueueCopyBufferToImage clEnqueueCopyBufferToImage;
	cl_api_clEnqueueMapBuffer clEnqueueMapBuffer;
	cl_api_clEnqueueMapImage clEnqueueMapImage;
	cl_api_clEnqueueUnmapMemObject clEnqueueUnmapMemObject;
	cl_api_clEnqueueNDRangeKernel clEnqueueNDRangeKernel;
	cl_api_clEnqueueTask clEnqueueTask;
	cl_api_clEnqueueNativeKernel clEnqueueNativeKernel;
	cl_api_clEnqueueMarker clEnqueueMarker;
	cl_api_clEnqueueWaitForEvents clEnqueueWaitForEvents;
	cl_api_clEnqueueBarrier clEnqueueBarrier;
	cl_api_clGetExtensionFunctionAddress clGetExtensionFunctionAddress;
	cl_api_clCreateFromGLBuffer clCreateFromGLBuffer;
	cl_api_clCreateFromGLTexture2D clCreateFromGLTexture2D;
	cl_api_clCreateFromGLTexture3D clCreateFromGLTexture3D;
	cl_api_clCreateFromGLRenderbuffer clCreateFromGLRenderbuffer;
	cl_api_clGetGLObjectInfo clGetGLObjectInfo;
	cl_api_clGetGLTextureInfo clGetGLTextureInfo;
	cl_api_clEnqueueAcquireGLObjects clEnqueueAcquireGLObjects;
	cl_api_clEnqueueReleaseGLObjects clEnqueueReleaseGLObjects;
	cl_api_clGetGLContextInfoKHR clGetGLContextInfoKHR;

	/* cl_khr_d3d10_sharing */
	cl_api_clGetDeviceIDsFromD3D10KHR clGetDeviceIDsFromD3D10KHR;
	cl_api_clCreateFromD3D10BufferKHR clCreateFromD3D10BufferKHR;
	cl_api_clCreateFromD3D10Texture2DKHR clCreateFromD3D10Texture2DKHR;
	cl_api_clCreateFromD3D10Texture3DKHR clCreateFromD3D10Texture3DKHR;
	cl_api_clEnqueueAcquireD3D10ObjectsKHR clEnqueueAcquireD3D10ObjectsKHR;
	cl_api_clEnqueueReleaseD3D10ObjectsKHR clEnqueueReleaseD3D10ObjectsKHR;

	/* OpenCL 1.1 */
	cl_api_clSetEventCallback clSetEventCallback;
	cl_api_clCreateSubBuffer clCreateSubBuffer;
	cl_api_clSetMemObjectDestructorCallback clSetMemObjectDestructorCallback;
	cl_api_clCreateUserEvent clCreateUserEvent;
	cl_api_clSetUserEventStatus clSetUserEventStatus;
	cl_api_clEnqueueReadBufferRect clEnqueueReadBufferRect;
	cl_api_clEnqueueWriteBufferRect clEnqueueWriteBufferRect;
	cl_api_clEnqueueCopyBufferRect clEnqueueCopyBufferRect;

	/* cl_ext_device_fission */
	cl_api_clCreateSubDevicesEXT clCreateSubDevicesEXT;
	cl_api_clRetainDeviceEXT clRetainDeviceEXT;
	cl_api_clReleaseDeviceEXT clReleaseDeviceEXT;

	/* cl_khr_gl_event */
	cl_api_clCreateEventFromGLsyncKHR clCreateEventFromGLsyncKHR;

	/* OpenCL 1.2 */
	cl_api_clCreateSubDevices clCreateSubDevices;
	cl_api_clRetainDevice clRetainDevice;
	cl_api_clReleaseDevice clReleaseDevice;
	cl_api_clCreateImage clCreateImage;
	cl_api_clCreateProgramWithBuiltInKernels clCreateProgramWithBuiltInKernels;
	cl_api_clCompileProgram clCompileProgram;
	cl_api_clLinkProgram clLinkProgram;
	cl_api_clUnloadPlatformCompiler clUnloadPlatformCompiler;
	cl_api_clGetKernelArgInfo clGetKernelArgInfo;
	cl_api_clEnqueueFillBuffer clEnqueueFillBuffer;
	cl_api_clEnqueueFillImage clEnqueueFillImage;
	cl_api_clEnqueueMigrateMemObjects clEnqueueMigrateMemObjects;
	cl_api_clEnqueueMarkerWithWaitList clEnqueueMarkerWithWaitList;
	cl_api_clEnqueueBarrierWithWaitList clEnqueueBarrierWithWaitList;
	cl_api_clGetExtensionFunctionAddressForPlatform
	    clGetExtensionFunctionAddressForPlatform;
	cl_api_clCreateFromGLTexture clCreateFromGLTexture;

	/* cl_khr_d3d11_sharing */
	cl_api_clGetDeviceIDsFromD3D11KHR clGetDeviceIDsFromD3D11KHR;
	cl_api_clCreateFromD3D11BufferKHR clCreateFromD3D11BufferKHR;
	cl_api_clCreateFromD3D11Texture2DKHR clCreateFromD3D11Texture2DKHR;
	cl_api_clCreateFromD3D11Texture3DKHR clCreateFromD3D11Texture3DKHR;
	cl_api_clCreateFromDX9MediaSurfaceKHR clCreateFromDX9MediaSurfaceKHR;
	cl_api_clEnqueueAcquireD3D11ObjectsKHR clEnqueueAcquireD3D11ObjectsKHR;
	cl_api_clEnqueueReleaseD3D11ObjectsKHR clEnqueueReleaseD3D11ObjectsKHR;

	/* cl_khr_dx9_media_sharing */
	cl_api_clGetDeviceIDsFromDX9MediaAdapterKHR
	    clGetDeviceIDsFromDX9MediaAdapterKHR;
	cl_api_clEnqueueAcquireDX9MediaSurfacesKHR
	    clEnqueueAcquireDX9MediaSurfacesKHR;
	cl_api_clEnqueueReleaseDX9MediaSurfacesKHR
	    clEnqueueReleaseDX9MediaSurfacesKHR;

	/* cl_khr_egl_image */
	cl_api_clCreateFromEGLImageKHR clCreateFromEGLImageKHR;
	cl_api_clEnqueueAcquireEGLObjectsKHR clEnqueueAcquireEGLObjectsKHR;
	cl_api_clEnqueueReleaseEGLObjectsKHR clEnqueueReleaseEGLObjectsKHR;

	/* cl_khr_egl_event */
	cl_api_clCreateEventFromEGLSyncKHR clCreateEventFromEGLSyncKHR;

	/* OpenCL 2.0 */
	cl_api_clCreateCommandQueueWithProperties clCreateCommandQueueWithProperties;
	cl_api_clCreatePipe clCreatePipe;
	cl_api_clGetPipeInfo clGetPipeInfo;
	cl_api_clSVMAlloc clSVMAlloc;
	cl_api_clSVMFree clSVMFree;
	cl_api_clEnqueueSVMFree clEnqueueSVMFree;
	cl_api_clEnqueueSVMMemcpy clEnqueueSVMMemcpy;
	cl_api_clEnqueueSVMMemFill clEnqueueSVMMemFill;
	cl_api_clEnqueueSVMMap clEnqueueSVMMap;
	cl_api_clEnqueueSVMUnmap clEnqueueSVMUnmap;
	cl_api_clCreateSamplerWithProperties clCreateSamplerWithProperties;
	cl_api_clSetKernelArgSVMPointer clSetKernelArgSVMPointer;
	cl_api_clSetKernelExecInfo clSetKernelExecInfo;

	/* cl_khr_sub_groups */
	cl_api_clGetKernelSubGroupInfoKHR clGetKernelSubGroupInfoKHR;

	/* OpenCL 2.1 */
	cl_api_clCloneKernel clCloneKernel;
	cl_api_clCreateProgramWithIL clCreateProgramWithIL;
	cl_api_clEnqueueSVMMigrateMem clEnqueueSVMMigrateMem;
	cl_api_clGetDeviceAndHostTimer clGetDeviceAndHostTimer;
	cl_api_clGetHostTimer clGetHostTimer;
	cl_api_clGetKernelSubGroupInfo clGetKernelSubGroupInfo;
	cl_api_clSetDefaultDeviceCommandQueue clSetDefaultDeviceCommandQueue;

	/* OpenCL 2.2 */
	cl_api_clSetProgramReleaseCallback clSetProgramReleaseCallback;
	cl_api_clSetProgramSpecializationConstant clSetProgramSpecializationConstant;

	/* OpenCL 3.0 */
	cl_api_clCreateBufferWithProperties clCreateBufferWithProperties;
	cl_api_clCreateImageWithProperties clCreateImageWithProperties;
	cl_api_clSetContextDestructorCallback clSetContextDestructorCallback;
#endif
};
