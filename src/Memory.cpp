#include "CLCPU/Memory.h"
#include "CLCPU/Getter.h"

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

static auto getMemObjectInfoFields = std::map<cl_mem_info, std::shared_ptr<Getter<cl_mem>>>{
	{CL_MEM_TYPE, GetField(&_cl_mem::type)},
	{CL_MEM_FLAGS, GetField(&_cl_mem::flags)},
	{CL_MEM_SIZE, GetField(&_cl_mem::size)},
	{CL_MEM_HOST_PTR, GetField(&_cl_mem::hostPtr)},
	{CL_MEM_MAP_COUNT, GetPrimitiveLiteral<cl_mem, cl_uint>(0)},
	{CL_MEM_CONTEXT, GetField(&_cl_mem::context)},
};

CL_API_ENTRY cl_int CL_API_CALL
clGetMemObjectInfo(
	cl_mem memobj,
	cl_mem_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	if (!verifyMem(memobj)) return CL_INVALID_PLATFORM;
	auto i = getMemObjectInfoFields.find(param_name);
	if (i == getMemObjectInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, memobj);
}
