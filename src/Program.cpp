#include "CLCPU/Program.h"
#include "CLCPU/Context.h"
#include "CLCPU/Device.h"
#include "CLCPU/Getter.h"
#include "Common/File.h"

#include <cstdlib>
#include <map>
#include <dlfcn.h>

int cl_program_verify = rand();
	
int _cl_program::lastID = 0;

std::map<cl_program, std::shared_ptr<_cl_program>> allPrograms;

struct vectorTypeInfo {
	vectorTypeInfo(
		std::string name_,
		size_t size_
	) : name(name_),
		size(size_)
	{}

	std::string name;
	size_t size = {};
};
	
//shared between symbol file and here
static std::string progLibVarsVarName = "program_lib_vars";

//what does the cl_program communicate with the library it links?
// TODO THIS MUST MATCH THE Program.cpp GENERATE CL CODE HEADER!!!
struct ProgramLibVars {
	size_t global_id[_cl_device_id::maxWorkItemDim];
	size_t global_size[_cl_device_id::maxWorkItemDim];
	size_t local_id[_cl_device_id::maxWorkItemDim];
	size_t local_size[_cl_device_id::maxWorkItemDim];
	size_t group_id[_cl_device_id::maxWorkItemDim];
};

#define MAKE_VECTOR_TYPE_INFO(x)	vectorTypeInfo(#x, sizeof(cl_##x))

std::vector<vectorTypeInfo> vectorTypes = {
	MAKE_VECTOR_TYPE_INFO(char),
	MAKE_VECTOR_TYPE_INFO(uchar),
	MAKE_VECTOR_TYPE_INFO(short),
	MAKE_VECTOR_TYPE_INFO(ushort),
	MAKE_VECTOR_TYPE_INFO(int),
	MAKE_VECTOR_TYPE_INFO(uint),
	MAKE_VECTOR_TYPE_INFO(long),
	MAKE_VECTOR_TYPE_INFO(ulong),
	MAKE_VECTOR_TYPE_INFO(float),
	MAKE_VECTOR_TYPE_INFO(double),
};

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithSource(
	cl_context context,
	cl_uint count,
	const char ** strings,
	const size_t * lengths,
	cl_int * errcode_ret
) {
	if (!verifyContext(context)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_CONTEXT;
		return nullptr;
	}

	int id = ++_cl_program::lastID;

	std::string code = "";
// should I put this #ifdef inside code?
#ifdef WINDOWS
	code += R"(
#define __attribute__(x)

//I hate Windows
#define EXTERN __declspec(dllexport)
#define kernel EXTERN

)";
#else
	code += R"(
#define EXTERN
#define kernel
)";
#endif
	
	code += R"(
#define constant
#define global
#define local

#if !defined(__cplusplus)
typedef char bool;
#define true 1
#define false 0
#endif

#include <stddef.h>

//unlike CL, C cannot handle function overloading
#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))
#define clamp(x,_min,_max)	min(_max,max(_min,x))

//TODO
#define CLK_LOCAL_MEM_FENCE	0
void barrier(int whatever) {}

#include <math.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

)";

// TODO half type too?
	for (auto const base : vectorTypes) {
		auto const name = base.name;
		auto const size = base.size;
code += R"(
typedef union {
	struct { )" + name + R"( x, y; };
	struct { )" + name + R"( s0, s1; };
	)" + name + R"( s[2];
} )" + name + R"(2 __attribute__((aligned()" + std::to_string(size*2) + R"()));

typedef union {
	struct { )" + name + R"( x, y, z; };
	struct { )" + name + R"( s0, s1, s2; };
	)" + name + R"( s[3];
} )" + name + R"(3 __attribute__((aligned()" + std::to_string(size*4) + R"()));

typedef union {
	struct { )" + name + R"( x, y, z, w; };
	struct { )" + name + R"( s0, s1, s2, s3; };
	)" + name + R"( s[4];
} )" + name + R"(4 __attribute__((aligned()" + std::to_string(size*4) + R"()));

#define _)" + name + R"(4(a,b,c,d) ()" + name + R"(4){.s={a,b,c,d}}

typedef union {
	struct { )" + name + R"( x, y, z, w; };
	struct { )" + name + R"( s0, s1, s2, s3, s4, s5, s6, s7; };
	)" + name + R"( s[8];
} )" + name + R"(8 __attribute__((aligned()" + std::to_string(size*8) + R"()));
)";
	}

// TODO THIS MUST MATCH THE Program.h FILE!!!
	std::vector<std::string> libVarFields = {
		"global_id",
		"global_size",
		"local_id",
		"local_size",
		"group_id",
	};

	//local to the cl file
	std::string progLibVarsTypeName = "ProgramLibVars";

	code += "struct " + progLibVarsTypeName + " {\n";
	for (const auto field : libVarFields) {
		code += "\tsize_t " + field + "[" + std::to_string(_cl_device_id::maxWorkItemDim) + "];\n";
	}
	code += "};\n\n";	

	std::string arrayInitCode = "{";
	for (size_t i = 0; i < _cl_device_id::maxWorkItemDim; ++i) {
		if (i > 0) arrayInitCode += ",";
		arrayInitCode += "0";
	}
	arrayInitCode += "}";

	code += "EXTERN " + progLibVarsTypeName + " " + progLibVarsVarName + " = {\n";
	for (const auto field : libVarFields) {
		code += "\t." + field + " = " + arrayInitCode + ",\n";
	}
	code += "};\n\n";
	
	for (const auto field : libVarFields) {
		code += "#define get_" + field + "(n)	(" + progLibVarsVarName + "." + field + "[n])\n";
	}

	code += R"(

int4 int4_add(int4 a, int4 b) {
	return (int4){
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
		.w = a.w + b.w,
	};
}

)";

	std::shared_ptr<_cl_program> program = std::make_shared<_cl_program>(
		context,
		id,
		code
	);
	auto programRaw = program.get();
	allPrograms[programRaw] = program;
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return programRaw;
}

CL_API_ENTRY cl_program CL_API_CALL
clCreateProgramWithBinary(
	cl_context context,
	cl_uint num_devices,
	const cl_device_id * device_list,
	const size_t * lengths,
	const unsigned char ** binaries,
	cl_int * binary_status,
	cl_int * errcode_ret
) {
	if (errcode_ret) *errcode_ret = CL_INVALID_VALUE;
	return nullptr;
}

bool verifyProgram(_cl_program* program) {
	if (!program) return false;
	if (program->verify != cl_program_verify) return false;
#if defined(VERIFY_MEMORY_EXTRA_STRICT)
	if (allPrograms.find(program) == allPrograms.end()) return false;
#endif
	return true;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainProgram(
	cl_program program
) {
	if (!verifyProgram(program)) return CL_INVALID_PROGRAM;
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseProgram(
	cl_program program
) {
	if (!verifyProgram(program)) return CL_INVALID_PROGRAM;
	return CL_SUCCESS;
}

/*
ok this is messy.

The multithread way: 
	start up a thread to do the building
	and don't assign the build status until done
	and let things run in concurrency

The singlthread way:
no extra thread. just block.
*/
CL_API_ENTRY cl_int CL_API_CALL
clBuildProgram(
	cl_program program,
	cl_uint num_devices,
	const cl_device_id * device_list,
	const char * options,
	void (CL_CALLBACK * pfn_notify)(
		cl_program program,
		void * user_data),
	void * user_data
) {
	if ((!device_list && num_devices) ||
		(device_list && !num_devices))
	{
		return CL_INVALID_VALUE;
	}
	
	if (!verifyProgram(program)) return CL_INVALID_PROGRAM;

	std::vector<cl_device_id> deviceVec;
	for (size_t i = 0; i < num_devices; ++i) {
		if (!verifyDevice(device_list[i])) return CL_INVALID_DEVICE;
		deviceVec.push_back(device_list[i]);
	}

	//TODO check build options, return CL_INVALID_BUILD_OPTIONS upon fail

	if (!pfn_notify && user_data) return CL_INVALID_VALUE;

	// TODO clear any build-specific program variables here
	program->options = {};
	program->log = {};
	program->devices = {};
	program->status = CL_BUILD_IN_PROGRESS;
	program->libfn = {};
	program->libvars = {};
	program->libhandle = {};
	
	// TODO and do the building here
	std::string id = std::to_string(program->id);
	std::string basename = "libtmp-1-" + id;
	std::string srcfn = basename + ".c";
	std::string objfn = basename + ".o";
	std::string libfn = basename + ".o";
	std::string logfn = basename + ".log";
	
	Common::File::write(srcfn, program->code);

	if (std::system((std::string() + "gcc -c -Wall -fPIC -O3 -std=c99 -o " + objfn + " " + srcfn + " > " + logfn + " 2>&1").c_str())) {
		program->status = CL_BUILD_ERROR;
		program->log = Common::File::read(logfn);
		return CL_BUILD_PROGRAM_FAILURE;
	}

	if (std::system((std::string() + "gcc -shared " + objfn + " -lm -o " + libfn + " >> " + logfn + " 2>&1").c_str())) {
		program->status = CL_BUILD_ERROR;
		program->log = Common::File::read(logfn);
		return CL_BUILD_PROGRAM_FAILURE;
	}

	void * libhandle = dlopen(libfn.c_str(), RTLD_LAZY);
	if (!libhandle) {
		program->status = CL_BUILD_ERROR;
		program->log = Common::File::read(logfn);
		return CL_BUILD_PROGRAM_FAILURE;
	}

	auto libvars = (ProgramLibVars*)dlsym(libhandle, progLibVarsVarName.c_str());
	if (!libvars) {
		program->status = CL_BUILD_ERROR;
		program->log = Common::File::read(logfn);
		return CL_BUILD_PROGRAM_FAILURE;
	}

	program->libfn = libfn;
	program->libhandle = libhandle;
	program->libvars = libvars;
	program->options = options;
	program->log = Common::File::read(logfn);
	program->devices = deviceVec;
	program->status = CL_BUILD_SUCCESS;
	return CL_SUCCESS;
}

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int CL_API_CALL
clUnloadCompiler() {
	return CL_SUCCESS;
}

static auto getProgramInfoFields = std::map<cl_program_info, std::shared_ptr<Getter<cl_program>>>{
	{CL_PROGRAM_CONTEXT, GetField(&_cl_program::context)},
	{CL_PROGRAM_NUM_DEVICES, std::make_shared<GetPrimitiveFromLambda<cl_program, cl_uint>>(
		[](cl_program program) -> cl_uint {
			return program->devices.size();
		}
	)},
	{CL_PROGRAM_DEVICES, std::make_shared<GetterLambda<cl_program>>(
		[](size_t param_value_size, void* param_value, size_t* param_value_size_ret, cl_program program) -> cl_int {
			using ResultType = cl_device_id;
			if (param_value_size_ret) {
				*param_value_size_ret = sizeof(ResultType) * program->devices.size();
			}
			if (param_value) {
				if (param_value_size < sizeof(ResultType) * program->devices.size()) {
					return CL_INVALID_VALUE;
				}
				for (size_t i = 0; i < program->devices.size(); ++i) {
					((ResultType*)param_value)[i] = program->devices[i];
				}
			}
			return CL_SUCCESS;	
		}
	)},
	{CL_PROGRAM_SOURCE, GetField(&_cl_program::code)},
	//CL_PROGRAM_BINARY_SIZES should return a size_t[]
	//CL_PROGRAM_BINARIES return an unsigned char*[] ... but do we allocate it?  with malloc?
	// 1.2:
	//[CL_PROGRAM_NUM_KERNELS] = size_t,
	//[CL_PROGRAM_KERNEL_NAMES] = char[],
	// 2.1:
	//[CL_PROGRAM_IL] = char[]
	// 2.2:
	//[CL_PROGRAM_SCOPE_GLOBAL_CTORS_PRESENT] = cl_bool,
	//[CL_PROGRAM_SCOPE_GLOBAL_DTORS_PRESENT] = cl_bool,
};

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramInfo(
	cl_program program,
	cl_program_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	if (!verifyProgram(program)) return CL_INVALID_PROGRAM;
	auto i = getProgramInfoFields.find(param_name);
	if (i == getProgramInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, program);
}

static auto getProgramBuildInfoFields = std::map<cl_program_build_info, std::shared_ptr<Getter<cl_program>>>{
	{CL_PROGRAM_BUILD_STATUS, GetField(&_cl_program::status)},
	{CL_PROGRAM_BUILD_OPTIONS, GetField(&_cl_program::options)},
	{CL_PROGRAM_BUILD_LOG, GetField(&_cl_program::log)},
	// 1.2:
	//[CL_PROGRAM_BINARY_TYPE] = cl_program_binary_type
	// one of:
	// CL_PROGRAM_BINARY_TYPE_NONE
	// CL_PROGRAM_BINARY_TYPE_COMPILED_OBJECT
	// CL_PROGRAM_BINARY_TYPE_LIBRARY
	// CL_PROGRAM_BINARY_TYPE_EXECUTABLE
	// 2.0:
	//[CL_PROGRAM_BUILD_GLOBAL_VARIABLE_TOTAL_SIZE] = size_t
};

CL_API_ENTRY cl_int CL_API_CALL
clGetProgramBuildInfo(
	cl_program program,
	cl_device_id device,
	cl_program_build_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	//luckily I'm ignoring device for now
	//but if I didn't, I might have to change Getter to support multiple 'id' args ...
	if (!verifyProgram(program)) return CL_INVALID_PROGRAM;
	auto i = getProgramBuildInfoFields.find(param_name);
	if (i == getProgramBuildInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, program);
}
