// honestly if my names are matching then all I need to do is use cl.h
// but ... should my names match? I think they shouldn't -- of anything for safety
#include "CLCPU/Dispatch.h"
#include "CLCPU/Platform.h"
#include "CLCPU/Device.h"
#include "CLCPU/Context.h"
#include "CLCPU/CommandQueue.h"
#include "CLCPU/Memory.h"
#include "CLCPU/Sampler.h"
#include "CLCPU/Event.h"

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

	//Event.h
	clWaitForEvents,
	clGetEventInfo,
	clRetainEvent,
	clReleaseEvent,
	clGetEventProfilingInfo,
	
	//CommandQueue.h
	clFlush,
	clFinish,
	clEnqueueReadBuffer,
	clEnqueueWriteBuffer,
	clEnqueueCopyBuffer,
	nullptr,//cl_api_clEnqueueReadImage clEnqueueReadImage;
	nullptr,//cl_api_clEnqueueWriteImage clEnqueueWriteImage;
	nullptr,//cl_api_clEnqueueCopyImage clEnqueueCopyImage;
	nullptr,//cl_api_clEnqueueCopyImageToBuffer clEnqueueCopyImageToBuffer;
	nullptr,//cl_api_clEnqueueCopyBufferToImage clEnqueueCopyBufferToImage;
	nullptr,//cl_api_clEnqueueMapBuffer clEnqueueMapBuffer;
	nullptr,//cl_api_clEnqueueMapImage clEnqueueMapImage;
	nullptr,//cl_api_clEnqueueUnmapMemObject clEnqueueUnmapMemObject;
	nullptr,//cl_api_clEnqueueNDRangeKernel clEnqueueNDRangeKernel;
	nullptr,//cl_api_clEnqueueTask clEnqueueTask;
	nullptr,//cl_api_clEnqueueNativeKernel clEnqueueNativeKernel;
	nullptr,//cl_api_clEnqueueMarker clEnqueueMarker;
	nullptr,//cl_api_clEnqueueWaitForEvents clEnqueueWaitForEvents;
	nullptr,//cl_api_clEnqueueBarrier clEnqueueBarrier;
	nullptr,//cl_api_clGetExtensionFunctionAddress clGetExtensionFunctionAddress;
	nullptr,//cl_api_clCreateFromGLBuffer clCreateFromGLBuffer;
	nullptr,//cl_api_clCreateFromGLTexture2D clCreateFromGLTexture2D;
	nullptr,//cl_api_clCreateFromGLTexture3D clCreateFromGLTexture3D;
	nullptr,//cl_api_clCreateFromGLRenderbuffer clCreateFromGLRenderbuffer;
	nullptr,//cl_api_clGetGLObjectInfo clGetGLObjectInfo;
	nullptr,//cl_api_clGetGLTextureInfo clGetGLTextureInfo;
	nullptr,//cl_api_clEnqueueAcquireGLObjects clEnqueueAcquireGLObjects;
	nullptr,//cl_api_clEnqueueReleaseGLObjects clEnqueueReleaseGLObjects;
	nullptr,//cl_api_clGetGLContextInfoKHR clGetGLContextInfoKHR;

	/* cl_khr_d3d10_sharing */
	nullptr,//cl_api_clGetDeviceIDsFromD3D10KHR clGetDeviceIDsFromD3D10KHR;
	nullptr,//cl_api_clCreateFromD3D10BufferKHR clCreateFromD3D10BufferKHR;
	nullptr,//cl_api_clCreateFromD3D10Texture2DKHR clCreateFromD3D10Texture2DKHR;
	nullptr,//cl_api_clCreateFromD3D10Texture3DKHR clCreateFromD3D10Texture3DKHR;
	nullptr,//cl_api_clEnqueueAcquireD3D10ObjectsKHR clEnqueueAcquireD3D10ObjectsKHR;
	nullptr,//cl_api_clEnqueueReleaseD3D10ObjectsKHR clEnqueueReleaseD3D10ObjectsKHR;

	/* OpenCL 1.1 */
	nullptr,//cl_api_clSetEventCallback clSetEventCallback;
	nullptr,//cl_api_clCreateSubBuffer clCreateSubBuffer;
	nullptr,//cl_api_clSetMemObjectDestructorCallback clSetMemObjectDestructorCallback;
	nullptr,//cl_api_clCreateUserEvent clCreateUserEvent;
	nullptr,//cl_api_clSetUserEventStatus clSetUserEventStatus;
	nullptr,//cl_api_clEnqueueReadBufferRect clEnqueueReadBufferRect;
	nullptr,//cl_api_clEnqueueWriteBufferRect clEnqueueWriteBufferRect;
	nullptr,//cl_api_clEnqueueCopyBufferRect clEnqueueCopyBufferRect;

	/* cl_ext_device_fission */
	nullptr,//cl_api_clCreateSubDevicesEXT clCreateSubDevicesEXT;
	nullptr,//cl_api_clRetainDeviceEXT clRetainDeviceEXT;
	nullptr,//cl_api_clReleaseDeviceEXT clReleaseDeviceEXT;

	/* cl_khr_gl_event */
	nullptr,//cl_api_clCreateEventFromGLsyncKHR clCreateEventFromGLsyncKHR;

	/* OpenCL 1.2 */
	nullptr,//cl_api_clCreateSubDevices clCreateSubDevices;
	nullptr,//cl_api_clRetainDevice clRetainDevice;
	nullptr,//cl_api_clReleaseDevice clReleaseDevice;
	nullptr,//cl_api_clCreateImage clCreateImage;
	nullptr,//cl_api_clCreateProgramWithBuiltInKernels clCreateProgramWithBuiltInKernels;
	nullptr,//cl_api_clCompileProgram clCompileProgram;
	nullptr,//cl_api_clLinkProgram clLinkProgram;
	nullptr,//cl_api_clUnloadPlatformCompiler clUnloadPlatformCompiler;
	nullptr,//cl_api_clGetKernelArgInfo clGetKernelArgInfo;
	clEnqueueFillBuffer,
#if 0	
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
