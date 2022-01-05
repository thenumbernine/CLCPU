#include "CLCPU/Context.h"
#include "CLCPU/Device.h"
#include "CLCPU/Dispatch.h"
#include "CLCPU/Getter.h"

#include <map>
#include <memory>

static int cl_context_verify = rand();

struct _cl_context {
	_cl_context(std::vector<cl_device_id> devices_) : devices(devices_) {}
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	int verify = cl_context_verify;
	std::vector<cl_device_id> devices;
};

static std::vector<std::shared_ptr<_cl_context>> allContexts;

CL_API_ENTRY cl_context CL_API_CALL
clCreateContext(
	const cl_context_properties * properties,
	cl_uint num_devices,
	const cl_device_id * devices,
	void (CL_CALLBACK * pfn_notify)(
		const char * errinfo,
		const void * private_info,
		size_t cb,
		void * user_data),
	void * user_data,
	cl_int * errcode_ret
) CL_API_SUFFIX__VERSION_1_0 {
	for (size_t i = 0; i < num_devices; ++i) {
		if (!verifyDevice(devices[i])) {
			if (errcode_ret) *errcode_ret = CL_INVALID_DEVICE;
			return nullptr;
		}
	}
	std::vector<cl_device_id> devicesVec(devices, devices + num_devices);
	std::shared_ptr<_cl_context> context = std::make_shared<_cl_context>(devicesVec);
	allContexts.push_back(context);
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return context.get();
}

bool verifyContext(const cl_context ctx) {
	if (ctx->verify != cl_context_verify) return false;
	if (std::find_if(allContexts.begin(), allContexts.end(), [ctx](auto octx) -> bool {
		return octx.get() == ctx;
	}) == allContexts.end()) return false;
	return true;
}

CL_API_ENTRY cl_context CL_API_CALL
clCreateContextFromType(
	const cl_context_properties * properties,
	cl_device_type device_type,
	void (CL_CALLBACK * pfn_notify)(
		const char * errinfo,
		const void * private_info,
		size_t cb,
		void * user_data),
	void * user_data,
	cl_int * errcode_ret
) CL_API_SUFFIX__VERSION_1_0 {
	//TODO can you do null for device of context?
	return clCreateContext(properties, allDevices.size(), allDevices.data(), pfn_notify, user_data, errcode_ret);
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainContext(
	cl_context context
) CL_API_SUFFIX__VERSION_1_0 {
	if (!verifyContext(context)) return CL_INVALID_CONTEXT;
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseContext(
	cl_context context
) CL_API_SUFFIX__VERSION_1_0 {
	if (!verifyContext(context)) return CL_INVALID_CONTEXT;
	return CL_SUCCESS;
}

static auto getContextInfoFields = std::map<cl_context_info, std::shared_ptr<Getter<cl_context>>>{
	{CL_CONTEXT_NUM_DEVICES, std::make_shared<GetPrimitiveFromLambda<cl_context, cl_uint>>(
		[](cl_context context) -> cl_uint {
			return context->devices.size();
		}
	)},
	{CL_CONTEXT_DEVICES, std::make_shared<GetterLambda<cl_context>>(
		[](size_t param_value_size, void* param_value, size_t* param_value_size_ret, cl_context context) -> cl_int {
			using ResultType = cl_device_id;
			if (param_value_size_ret) {
				*param_value_size_ret = sizeof(ResultType) * context->devices.size();
			}
			if (param_value) {
				if (param_value_size < sizeof(ResultType) * context->devices.size()) {
					return CL_INVALID_VALUE;
				}
				for (size_t i = 0; i < context->devices.size(); ++i) {
					((ResultType*)param_value)[i] = context->devices[i];
				}
			}
			return CL_SUCCESS;	
		}
	)},
	{CL_CONTEXT_PROPERTIES, std::make_shared<GetterLambda<cl_context>>(
		[](size_t param_value_size, void* param_value, size_t* param_value_size_ret, cl_context context) -> cl_int {
			if (param_value_size_ret) {
				param_value_size_ret[0] = 0;
			}
			return CL_SUCCESS;	
		}
	)},
};

CL_API_ENTRY cl_int CL_API_CALL
clGetContextInfo(
	cl_context context,
	cl_context_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) CL_API_SUFFIX__VERSION_1_0 {
	if (!verifyContext(context)) return CL_INVALID_PLATFORM;
	auto i = getContextInfoFields.find(param_name);
	if (i == getContextInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, context);
}
