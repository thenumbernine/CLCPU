#include "CLCPU/Device.h"
#include "CLCPU/Platform.h"
#include "CLCPU/Dispatch.h"
#include "CLCPU/Getter.h"

#include <map>
#include <memory>
#include <thread>

int cl_device_verify = rand();

static _cl_device_id thisDevice;

std::vector<cl_device_id> allDevices = {&thisDevice};

CL_API_ENTRY cl_int CL_API_CALL clGetDeviceIDs(
	cl_platform_id platform,
	cl_device_type device_type,
	cl_uint num_entries,
	cl_device_id * devices,
	cl_uint * num_devices
) {
	if (!verifyPlatform(platform)) return CL_INVALID_PLATFORM;

	// TODO care about device_type
	
	if (!num_entries && devices) {
		return CL_INVALID_VALUE;
	}
	if (!num_devices && !devices) {
		return CL_INVALID_VALUE;
	}
	if (num_devices) {
		*num_devices = 1;
	}
	if (devices) {
		// no need to do anything, since num_devices should be zero
		if (num_entries >= 1) {
			thisDevice.platform = platform;
			devices[0] = &thisDevice;
		}
	}
	return CL_SUCCESS;
}
		
static cl_uint clDeviceMaxComputeUnits = std::thread::hardware_concurrency();

static auto getDeviceInfoFields = std::map<cl_device_info, std::shared_ptr<Getter<cl_device_id>>>{
	{CL_DEVICE_TYPE, GetPrimitiveLiteral<cl_device_id, cl_device_type>(CL_DEVICE_TYPE_CPU)},
	{CL_DEVICE_VENDOR_ID, GetPrimitiveLiteral<cl_device_id, cl_uint>(0)},
	{CL_DEVICE_MAX_COMPUTE_UNITS, GetPrimitiveLiteral<cl_device_id, cl_uint>(clDeviceMaxComputeUnits)},
	{CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, GetPrimitiveLiteral<cl_device_id, cl_uint>(_cl_device_id::maxWorkItemDim)},
	
	// TODO std::array-based getter
	{CL_DEVICE_MAX_WORK_ITEM_SIZES, std::make_shared<GetterLambda<cl_device_id>>(
		[](size_t param_value_size, void* param_value, size_t* param_value_size_ret, cl_device_id id) -> cl_int {
			using ResultType = size_t;
			if (param_value_size_ret) {
				*param_value_size_ret = sizeof(ResultType) * _cl_device_id::maxWorkItemDim;
			}
			if (param_value) {
				if (param_value_size < sizeof(ResultType) * _cl_device_id::maxWorkItemDim) {
					return CL_INVALID_VALUE;
				}
				((size_t*)param_value)[0] = 1024;
				((size_t*)param_value)[1] = 1024;
				((size_t*)param_value)[2] = 1024;
			}
			return CL_SUCCESS;
		}
	)},
	
	{CL_DEVICE_MAX_MEM_ALLOC_SIZE, GetPrimitiveLiteral<cl_device_id, cl_ulong>(_cl_device_id::maxMemAllocSize)},
	{CL_DEVICE_NAME, GetStringLiteral<cl_device_id>("CPU debug implementation")},
	{CL_DEVICE_VENDOR, GetStringLiteral<cl_device_id>("Christopher Moore")},
	{CL_DEVICE_PROFILE, GetStringLiteral<cl_device_id>("FULL_PROFILE")},
	{CL_DEVICE_VERSION, GetStringLiteral<cl_device_id>("OpenCL 1.1")},
	{CL_DEVICE_EXTENSIONS, GetStringLiteral<cl_device_id>("cl_khr_fp64")},
	{CL_DEVICE_PLATFORM, GetField(&_cl_device_id::platform)},
	{CL_DEVICE_OPENCL_C_VERSION, GetStringLiteral<cl_device_id>("OpenCL 1.1")},
	{CL_DEVICE_LINKER_AVAILABLE, GetPrimitiveLiteral<cl_device_id, cl_bool>(false)},
	{CL_DEVICE_BUILT_IN_KERNELS, GetStringLiteral<cl_device_id>("")},
};

bool verifyDevice(const cl_device_id device) {
	if (!device) return false;
	if (device->verify != cl_device_verify) return false;
	if (device != &thisDevice) return false;
	return true;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceInfo(
	cl_device_id device,
	cl_device_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	if (!verifyDevice(device)) return CL_INVALID_PLATFORM;
	auto i = getDeviceInfoFields.find(param_name);
	if (i == getDeviceInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, device);
}
