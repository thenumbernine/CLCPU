#include "CLCPU/Platform.h"
#include "CLCPU/Dispatch.h"
#include "CLCPU/Getter.h"

#include <map>
#include <memory>
#include <chrono>

// https://www.khronos.org/registry/OpenCL/specs/2.2/html/OpenCL_Ext.html#clGetExtensionFunctionAddressForPlatform
// 2.7 

static int cl_platform_verify = rand();

/* 2.2 says to always put "struct _cl_icd_dispatch *dispatch" first ...
but what is struct _cl_icd_dispatch *dispatch ?
here's our best hint:

	The structure _cl_icd_dispatch is a function pointer dispatch table which is used to direct calls to a particular vendor implementation. All objects created from ICD compatible objects must be ICD compatible.
	The order of the functions in _cl_icd_dispatch is determined by the ICD Loader’s source. The ICD Loader’s source’s _cl_icd_dispatch table is to be appended to only.

how about a struct/function/anything for loading function pointers from the icd-loaded shared library?
or does libOpenCL just look for identical named functions in the icd-loaded .so/.dll?
*/
struct _cl_platform_id {
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	int verify = cl_platform_verify;
};
static_assert(offsetof(_cl_platform_id, dispatch) == 0);

static _cl_platform_id thisPlatform;

CL_API_ENTRY cl_int CL_API_CALL clGetPlatformIDs(
	cl_uint num_entries,
	cl_platform_id *platforms,
	cl_uint *num_platforms
) {
	if (!num_entries && platforms) {
		return CL_INVALID_VALUE;
	}
	if (!num_platforms && !platforms) {
		return CL_INVALID_VALUE;
	}
	if (num_platforms) {
		*num_platforms = 1;
	}
	if (platforms) {
		// no need to do anything, since num_platforms should be zero
		if (num_entries >= 1) {
			platforms[0] = &thisPlatform;
		}
	}
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL clIcdGetPlatformIDsKHR(
	cl_uint num_entries,
	cl_platform_id *platforms,
	cl_uint *num_platforms
) {
	return clGetPlatformIDs(num_entries, platforms, num_platforms);
}

// hmm do I want to go constant-time access with compile-time getter, or log-n-time access with runtime virtual getter?
static auto getPlatformInfoFields = std::map<cl_platform_info, std::shared_ptr<Getter<cl_platform_id>>>{
	{CL_PLATFORM_PROFILE, std::make_shared<GetStringLiteral<cl_platform_id>>("FULL_PROFILE")},
	{CL_PLATFORM_VERSION, std::make_shared<GetStringLiteral<cl_platform_id>>("OpenCL 1.1")},
	{CL_PLATFORM_NAME, std::make_shared<GetStringLiteral<cl_platform_id>>("CPU debug implementation")},
	{CL_PLATFORM_VENDOR, std::make_shared<GetStringLiteral<cl_platform_id>>("Christopher Moore")},
	{CL_PLATFORM_EXTENSIONS, std::make_shared<GetStringLiteral<cl_platform_id>>("")},
	{CL_PLATFORM_HOST_TIMER_RESOLUTION, std::make_shared<GetPrimitiveFromLambda<cl_platform_id, cl_ulong>>([](cl_platform_id) -> cl_ulong {
		// https://stackoverflow.com/questions/8386128/how-to-get-the-precision-of-high-resolution-clock
		return std::chrono::duration<cl_ulong, std::nano>(typename std::chrono::high_resolution_clock::duration(1)).count();

		// https://en.cppreference.com/w/cpp/chrono/duration
		/*
		return std::chrono::nanoseconds(
			std::chrono::high_resolution_clock::period::num 
			/ std::chrono::high_resolution_clock::period::den
		).count();
		*/
	})},
};

bool verifyPlatform(const cl_platform_id platform) {
	//verify platform
	if (platform->verify != cl_platform_verify) {
		return false;
	}
	if (platform != &thisPlatform) {
		return false;
	}
	return true;
}

CL_API_ENTRY cl_int CL_API_CALL clGetPlatformInfo(
	cl_platform_id platform,
	cl_platform_info param_name,
	size_t param_value_size,
	void* param_value,
	size_t* param_value_size_ret
) {
	if (!verifyPlatform(platform)) return CL_INVALID_PLATFORM;
	auto i = getPlatformInfoFields.find(param_name);
	if (i == getPlatformInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, platform);
}

// deprecated but still required
void* clGetExtensionFunctionAddress(const char *funcname) {
	return nullptr;
}

// what is this supposed to do?
void* clGetExtensionFunctionAddressForPlatform(
	cl_platform_id platform,
	const char *funcname
) {
	return nullptr;
}
