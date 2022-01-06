#include "CLCPU/Event.h"

CL_API_ENTRY cl_int CL_API_CALL
clWaitForEvents(
	cl_uint num_events,
	const cl_event * event_list
) {
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetEventInfo(
	cl_event event,
	cl_event_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	return CL_INVALID_VALUE;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainEvent(cl_event event) {
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseEvent(cl_event event) {
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetEventProfilingInfo(
	cl_event event,
	cl_profiling_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	return CL_INVALID_VALUE;
}
