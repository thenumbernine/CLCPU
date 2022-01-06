#include "CLCPU/CommandQueue.h"
#include "CLCPU/Context.h"
#include "CLCPU/Device.h"
#include "CLCPU/Memory.h"
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
	auto queueRaw = queue.get();
	allQueues[queueRaw] = queue;
	if (!device->defaultCommandQueue) device->defaultCommandQueue = queueRaw;
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return queueRaw;
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

CL_API_ENTRY cl_int CL_API_CALL
clFlush(cl_command_queue command_queue) {
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clFinish(cl_command_queue command_queue) {
	return CL_SUCCESS;
}

static void handleEvents(
	cl_uint num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event * event
) {
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReadBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_read,
	size_t offset,
	size_t size,
	void * ptr,
	cl_uint num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event * event
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	if (num_events_in_wait_list > 0 && !event_wait_list) return CL_INVALID_EVENT_WAIT_LIST;
	handleEvents(num_events_in_wait_list, event_wait_list, event);
	if (!ptr) return CL_INVALID_VALUE;
	if (!verifyMem(buffer)) return CL_INVALID_MEM_OBJECT;
	if (offset + size > buffer->buffer.size()) return CL_INVALID_VALUE;
	auto byteptr = (uint8_t*)ptr;
	std::copy(buffer->buffer.begin(), buffer->buffer.end(), byteptr); 
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueWriteBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	cl_bool blocking_write,
	size_t offset,
	size_t size,
	const void * ptr,
	cl_uint num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event * event
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	if (num_events_in_wait_list > 0 && !event_wait_list) return CL_INVALID_EVENT_WAIT_LIST;
	handleEvents(num_events_in_wait_list, event_wait_list, event);
	if (!ptr) return CL_INVALID_VALUE;
	if (!verifyMem(buffer)) return CL_INVALID_MEM_OBJECT;
	if (offset + size > buffer->buffer.size()) return CL_INVALID_VALUE;
	auto byteptr = (uint8_t*)ptr;
	std::copy(byteptr, byteptr + size, buffer->buffer.begin()); 
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clEnqueueCopyBuffer(
	cl_command_queue command_queue,
	cl_mem src_buffer,
	cl_mem dst_buffer,
	size_t src_offset,
	size_t dst_offset,
	size_t size,
	cl_uint num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event * event
) {
	throw std::runtime_error("TODO");
}

// wow, only CL 1.2 has FillBuffer ... smh
CL_API_ENTRY cl_int CL_API_CALL
clEnqueueFillBuffer(
	cl_command_queue command_queue,
	cl_mem buffer,
	const void * pattern,
	size_t pattern_size,
	size_t offset,
	size_t size,
	cl_uint num_events_in_wait_list,
	const cl_event * event_wait_list,
	cl_event * event
) {
	if (!verifyQueue(command_queue)) return CL_INVALID_COMMAND_QUEUE;
	if (num_events_in_wait_list > 0 && !event_wait_list) return CL_INVALID_EVENT_WAIT_LIST;
	handleEvents(num_events_in_wait_list, event_wait_list, event);
	if (!verifyMem(buffer)) return CL_INVALID_MEM_OBJECT;
	
	if (!pattern) return CL_INVALID_VALUE;
	if (pattern_size != 1 &&
		pattern_size != 2 &&
		pattern_size != 4 &&
		pattern_size != 8 &&
		pattern_size != 16 &&
		pattern_size != 32 &&
		pattern_size != 64 &&
		pattern_size != 128)
	{
		return CL_INVALID_VALUE;
	}
	if (size % pattern_size != 0) return CL_INVALID_VALUE;
	
	if (offset + size > buffer->buffer.size()) return CL_INVALID_VALUE;
	
	auto bytepattern = (uint8_t*)pattern;
	bool isZero = true;
	for (size_t i = 0; i < pattern_size; ++i) {
		if (bytepattern[i] != 0) {
			isZero = false;
			break;
		}
	}
	if (isZero) {
		std::fill(buffer->buffer.begin(), buffer->buffer.end(), 0);
	} else {
		for (size_t i = 0; i < size; ++i) {
			buffer->buffer[offset+i] = bytepattern[i%pattern_size];
		}
	}

	return CL_SUCCESS;
}
