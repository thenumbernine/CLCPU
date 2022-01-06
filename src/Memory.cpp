#include "CLCPU/Memory.h"

#include <map>
#include <memory>

int cl_mem_verify = rand();

static std::map<_cl_mem*, std::shared_ptr<_cl_mem>> allMems;

bool verifyMem(_cl_mem* mem) {
	if (!mem) return false;
	if (mem->verify != cl_mem_verify) return false;
#if defined(VERIFY_MEMORY_EXTRA_STRICT)
	if (allMems.find(mem) == allMems.end()) return false;
#endif
	return true;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainMemObject(cl_mem memobj) {
	if (!verifyMem(memobj)) return CL_INVALID_MEM_OBJECT;
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseMemObject(cl_mem memobj) {
	if (!verifyMem(memobj)) return CL_INVALID_MEM_OBJECT;
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(
	cl_mem memobj,
	cl_mem_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	return CL_INVALID_MEM_OBJECT;
}
