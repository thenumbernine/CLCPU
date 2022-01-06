#include "CLCPU/Memory.h"
#include "CLCPU/Context.h"
#include "CLCPU/Device.h"
#include "CLCPU/Getter.h"

#include <map>
#include <memory>

int cl_mem_verify = rand();

static std::map<_cl_mem*, std::shared_ptr<_cl_mem>> allMems;

CL_API_ENTRY cl_mem CL_API_CALL
clCreateBuffer(
	cl_context context,
	cl_mem_flags flags,
	size_t size,
	void * host_ptr,
	cl_int * errcode_ret
) {
	if (!verifyContext(context)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_CONTEXT;
		return nullptr;
	}
	
	if (flags & ~(
		CL_MEM_READ_WRITE |
		CL_MEM_WRITE_ONLY |
		CL_MEM_READ_ONLY |
		CL_MEM_USE_HOST_PTR |
		CL_MEM_ALLOC_HOST_PTR |
		CL_MEM_COPY_HOST_PTR
	)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_VALUE;
		return nullptr;
	}
	
	if (!size || size > _cl_device_id::maxMemAllocSize) {
		if (errcode_ret) *errcode_ret = CL_INVALID_BUFFER_SIZE;
		return nullptr;
	}

	bool reqHost = flags & (CL_MEM_USE_HOST_PTR | CL_MEM_COPY_HOST_PTR);
	if ((reqHost && !host_ptr) || (!reqHost && host_ptr)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_HOST_PTR;
		return nullptr;
	}

	std::shared_ptr<_cl_mem> mem = std::make_shared<_cl_mem>(
		size,
		flags,
		(uint8_t*)host_ptr,
		context
	);
	allMems[mem.get()] = mem;
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return mem.get();
}

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
	{CL_MEM_SIZE, std::make_shared<GetPrimitiveFromLambda<cl_mem, size_t>>(
		[](cl_mem mem) -> size_t {
			return mem->buffer.size();
		}
	)},
	{CL_MEM_HOST_PTR, GetField(&_cl_mem::host_ptr)},
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
