#include "CLCPU/CommandQueue.h"
#include "CLCPU/Context.h"
#include "CLCPU/Device.h"
#include "CLCPU/Dispatch.h"
#include "CLCPU/Getter.h"

#include <map>
#include <memory>

static int cl_queue_verify = rand();

struct _cl_command_queue {
	_cl_command_queue(cl_context ctx_, cl_device_id device_) : context(ctx_), device(device_) {}
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	int verify = cl_queue_verify;
	cl_context context = {};
	cl_device_id device = {};
	
	//TODO instead of size, have the actual list of commands
	//and have a thread running that is executing them.
	cl_uint size = {};
};
static_assert(offsetof(_cl_command_queue, dispatch) == 0);

static std::map<_cl_command_queue*, std::shared_ptr<_cl_command_queue>> allQueues;

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_command_queue CL_API_CALL
clCreateCommandQueue(
	cl_context context,
	cl_device_id device,
	cl_command_queue_properties properties,
	cl_int * errcode_ret
) {
	if (!verifyContext(context)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_CONTEXT;
		return nullptr;
	}
	if (!verifyDevice(device)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_DEVICE;
		return nullptr;
	}

	std::shared_ptr<_cl_command_queue> queue = std::make_shared<_cl_command_queue>(context, device);
	allQueues[queue.get()] = queue;
	if (!device->defaultCommandQueue) device->defaultCommandQueue = queue.get();
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return queue.get();
}

bool verifyQueue(const cl_command_queue queue) {
	if (!queue) return false;
	if (queue->verify != cl_queue_verify) return false;
	if (allQueues.find(queue) == allQueues.end()) return false;
	return true;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainCommandQueue(
	cl_command_queue command_queue
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseCommandQueue(
	cl_command_queue command_queue
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	return CL_SUCCESS;
}

static auto getCommandQueueInfoFields = std::map<cl_command_queue_info, std::shared_ptr<Getter<cl_command_queue>>>{
	{CL_QUEUE_CONTEXT, GetField(&_cl_command_queue::context)},
	{CL_QUEUE_DEVICE, GetField(&_cl_command_queue::device)},
	{CL_QUEUE_SIZE, GetField(&_cl_command_queue::size)},
	{CL_QUEUE_DEVICE_DEFAULT, std::make_shared<GetPrimitiveFromLambda<cl_command_queue, cl_command_queue>>(
		[](cl_command_queue queue) -> cl_command_queue {
			auto device = queue->device;
			if (!verifyDevice(device)) return nullptr;	// ? or return an error?
			return device->defaultCommandQueue;
		}
	)},
};

CL_API_ENTRY cl_int CL_API_CALL
clGetCommandQueueInfo(
	cl_command_queue command_queue,
	cl_command_queue_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	auto i = getCommandQueueInfoFields.find(param_name);
	if (i == getCommandQueueInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, command_queue);
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clSetCommandQueueProperty(
	cl_command_queue command_queue,
	cl_command_queue_properties properties,
	cl_bool enable,
	cl_command_queue_properties * old_properties
) {
	return CL_SUCCESS;
}
