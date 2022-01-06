#pragma once

#include "CLCPU/Dispatch.h"
#include <CL/cl.h>
#include <vector>
#include <string>
#include <memory>

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(cl_context        context,
                          cl_uint           count,
                          const char **     strings,
                          const size_t *    lengths,
                          cl_int *          errcode_ret) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithBinary(cl_context                     context,
                          cl_uint                        num_devices,
                          const cl_device_id *           device_list,
                          const size_t *                 lengths,
                          const unsigned char **         binaries,
                          cl_int *                       binary_status,
                          cl_int *                       errcode_ret) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clRetainProgram(cl_program program) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clReleaseProgram(cl_program program) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(cl_program           program,
               cl_uint              num_devices,
               const cl_device_id * device_list,
               const char *         options,
               void (CL_CALLBACK *  pfn_notify)(cl_program program,
                                                void * user_data),
               void *               user_data) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clUnloadCompiler(void) CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramInfo(cl_program         program,
                 cl_program_info    param_name,
                 size_t             param_value_size,
                 void *             param_value,
                 size_t *           param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramBuildInfo(cl_program            program,
                      cl_device_id          device,
                      cl_program_build_info param_name,
                      size_t                param_value_size,
                      void *                param_value,
                      size_t *              param_value_size_ret) CL_API_SUFFIX__VERSION_1_0;

extern int cl_program_verify;

struct ProgramLibVars;

struct _cl_program {

	static int lastID;
	
	_cl_program(
		cl_context context_,
		int id_,
		std::string code_
	)
	:	context(context_),
		id(id_),
		code(code_)
	{}

	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	int verify = cl_program_verify;
	cl_context context = {};
	int id = {};			// ID is used to prevent any symbol collisions by each linked program.  necessary?
	std::string code;
	cl_build_status status = CL_BUILD_NONE;
	std::string options;
	std::string log;
	// TODO any handles to the library HERE
	std::vector<cl_device_id> devices;

	//shared_ptr because program owns the kernels
	std::vector<std::shared_ptr<_cl_kernel>> kernels;

	//library filename:
	std::string libfn;
	
	//library handle:
	void * libhandle = {};

	//pointers inside the library:
	ProgramLibVars * libvars = {};
};
static_assert(offsetof(_cl_program, dispatch) == 0);
