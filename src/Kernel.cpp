#include "CLCPU/Kernel.h"
#include "CLCPU/Program.h"
#include "CLCPU/Memory.h"
#include "CLCPU/Device.h"
#include "CLCPU/Dispatch.h"
#include "CLCPU/Getter.h"

#include <string>
#include <vector>
#include <regex>
#include <dlfcn.h>

struct ArgInfo {
	std::string name;
	
	//if we get a pointer then treat the type as void*, but save the orig here
	// this will have global constant local stripped
	std::string origtype;	

	//type.  if it is global, constant, local then the type will be void*
	std::string type;
	
	bool isGlobal = {};
	bool isLocal = {};
	bool isConstant = {};
	
	std::vector<uint8_t> value;	//where to copy value into.  resize to be appropriate size.
};

static int cl_kernel_verify = rand();

struct _cl_kernel {
	_cl_kernel(
		std::string name_,
		cl_program program_,
		void * func_
	) : name(name_),
		program(program_),
		context(program_->context),
		func(func_)
	{}
	
	struct _cl_icd_dispatch const * const dispatch = &dispatchTable;
	int verify = cl_kernel_verify;
	std::string name;
	cl_program program = {};
	cl_context context = {};
	void * func = {};
	std::vector<ArgInfo> args;
};
static_assert(offsetof(_cl_kernel, dispatch) == 0);

// from my lua-preproc project ...
static auto removeCommentsAndApplyContinuations(std::string code) {

	// should line continuations \ affect single-line comments?
	// if so then do this here
	// or should they not?  then do this after.
	code = std::regex_replace(code, std::regex("\\\\\\s*\n"), " ");

	// remove all /* */ blocks first
	std::regex startregex = std::regex("\\/\\*");
	std::regex endregex = std::regex("\\*\\/");
	for (;;) {
		std::smatch startresult;
		if (!std::regex_search(code, startresult, startregex)) break;
		size_t startpos = startresult.position();
	
		//hmm, using code.substr itself in regex_search() caused compiler errors
		//only after assigning it to a std::string did it work
		std::string aftercommentstart = code.substr(startpos + 2);
		std::smatch endresult;
		if (!std::regex_search(aftercommentstart, endresult, endregex)) {
			throw std::runtime_error("found /* with no */");
		}
		size_t endpos = startpos + 2 + endresult.position() + 2;
		
		code = code.substr(0, startpos) + code.substr(endpos);
	}

	// remove all // \n blocks first
	code = std::regex_replace(code, std::regex("//[^\r\n]*[\r\n]"), "");

	return code;
}

//this should exist somewhere else, right?
std::vector<std::string> strsplit(const std::string &s, char delim) {
    std::vector<std::string> words;
    std::stringstream ss(s);
    std::string word;
    while(std::getline(ss, word, delim)) {
        words.push_back(word);
    }
    return words;
}

std::string strtrim(std::string s) {
	int n = s.length();
	int i = 0;
	for (; i < n && std::isspace(s[i]); ++i);
	if (i == n) return "";
	int j = n-1;
	for (; j >= 0 && !std::isspace(s[j]); --j);
	if (j <= i) return "";
	return s.substr(i, j-i);
}

std::string strveccat(std::vector<std::string> const & v, std::string const delim) {
	size_t n = v.size();
	if (!n) return "";
	std::string result = v[0];
	for (size_t i = 1; i < n; ++i) {
		result += delim;
		result += v[i];
	}
	return result;
}

CL_API_ENTRY cl_kernel CL_API_CALL
clCreateKernel(
	cl_program program,
	const char * kernel_name,
	cl_int * errcode_ret
) {
	if (!kernel_name) {
		if (errcode_ret) *errcode_ret = CL_INVALID_VALUE;
		return nullptr;
	}
	std::string kernelNameStr = kernel_name;

	if (!verifyProgram(program)) {
		if (errcode_ret) *errcode_ret = CL_INVALID_PROGRAM;
		return nullptr;
	}

	if (program->status != CL_BUILD_SUCCESS ||
		!program->libhandle
	) {
		if (errcode_ret) *errcode_ret = CL_INVALID_PROGRAM_EXECUTABLE;
		return nullptr;
	}

	// TODO here determine the argument types, either from the code or from something like ffi ctypes library
	// TODO TODO use libffi? or can libffi detect signatures?  or only link to them at runtime if they are already known?
	// until then I'll just parse things myself, like I'm doing in luajit ...

	std::string code = removeCommentsAndApplyContinuations(program->code);

	//any valid C name won't generate any regex patterns, right?
	std::smatch sigArgsMatch;
	if (!std::regex_search(code, sigArgsMatch, std::regex("kernel\\s\\s*void\\s\\s*" + kernelNameStr + "\\s*([^)]*)"))) {
		if (errcode_ret) *errcode_ret = CL_INVALID_KERNEL_NAME;
		return nullptr;
	}

	int i = 0;
	for (auto argstr : strsplit(code.substr(sigArgsMatch.position(), sigArgsMatch.length()), ',')) {
		ArgInfo arginfo;
		
		argstr = strtrim(argstr);
		auto tokens = strsplit(argstr, ',');
		for (size_t i = 0; i < tokens.size(); ++i) {
			tokens[i] = strtrim(tokens[i]);
		}
		//TODO and split extra any * prefixes or suffixes

		for (int j = (int)tokens.size()-1; j >= 0; --j) {
		
			for (;;) {
				auto k = tokens[j].find('*');
				if (k == std::string::npos) break;
				
				if (k < tokens[j].length()-1) {
					tokens.insert(tokens.begin() + j + 1, tokens[j].substr(k + 1));
				}
				tokens.insert(tokens.begin() + j + 1, "*");
				if (k > 0) {
					tokens.insert(tokens.begin() + j + 1, tokens[j].substr(0, k));
				}
			}

			if (tokens[j] == "global") {
				arginfo.isGlobal = true;
				tokens.erase(tokens.begin() + j);
			} else if (tokens[j] == "local") {
				arginfo.isLocal = true;
				tokens.erase(tokens.begin() + j);
			} else if (tokens[j] == "constant") {
				arginfo.isConstant = true;
				tokens.erase(tokens.begin() + j);
			}
		}

		arginfo.name = *tokens.rbegin();
		tokens.erase(tokens.end()-1);
		
		std::string origtype = strveccat(tokens, " ");
		arginfo.origtype = origtype;

		arginfo.type = origtype;
		if (std::find(tokens.begin(), tokens.end(), "*") != tokens.end()) {
			arginfo.type = "void *";
		}
		
		++i;
	}

	auto func = dlsym(program->libhandle, kernelNameStr.c_str());
	if (!func) {
		if (errcode_ret) *errcode_ret = CL_INVALID_KERNEL_NAME;
		return nullptr;
	}

	auto kernel = std::make_shared<_cl_kernel>(kernelNameStr, program, func);
	program->kernels[kernelNameStr] = kernel;
	
	if (errcode_ret) *errcode_ret = CL_SUCCESS;
	return kernel.get();
}

CL_API_ENTRY cl_int CL_API_CALL
clCreateKernelsInProgram(
	cl_program program,
	cl_uint num_kernels,
	cl_kernel * kernels,
	cl_uint * num_kernels_ret
) {
	//TODO query all kernels in a program
	//how to do this?
	//parsing through the code I guess
	return CL_INVALID_VALUE;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainKernel(cl_kernel kernel) {
	return CL_SUCCESS;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseKernel(cl_kernel kernel) {
	return CL_SUCCESS;
}

bool verifyKernel(_cl_kernel* kernel) {
	if (!kernel) return false;
	if (kernel->verify != cl_kernel_verify) return false;
#if defined(VERIFY_MEMORY_EXTRA_STRICT)
	bool found = false;
	for (auto p : allPrograms) {
		auto program = p.second;
		for (auto q : program->kernels) {
			auto okernel = q.second;
			if (okernel.get() == kernel) {
				found = true;
				break;
			}
		}
		if (found) break;
	}
	if (!found) return false;
#endif
	return true;
}


CL_API_ENTRY cl_int CL_API_CALL
clSetKernelArg(
	cl_kernel kernel,
	cl_uint arg_index,
	size_t arg_size,
	const void * arg_value
) {
	if (!verifyKernel(kernel)) return CL_INVALID_KERNEL;
	if (arg_index >= kernel->args.size()) return CL_INVALID_ARG_INDEX;
	auto & arginfo = kernel->args[arg_index];
	if (!arg_value && !arginfo.isLocal) return CL_INVALID_ARG_VALUE;
	
	if (arginfo.isGlobal || arginfo.isConstant) {
		if (!arg_value) return CL_INVALID_MEM_OBJECT;
		if (!verifyMem(*(cl_mem*)arg_value)) return CL_INVALID_MEM_OBJECT;
		if (arg_size < sizeof(cl_mem)) return CL_INVALID_ARG_SIZE;
	} else {
		//TODO need arginfo.type_size or sizeof(arginfo.type)
		//if (arg_size < arginfo.type_size && !arginfo.isLocal) return CL_INVALID_ARG_SIZE;
	}
	
	arginfo.value.resize(arg_size);
	std::copy(arginfo.value.begin(), arginfo.value.end(), (uint8_t*)arg_value);

	return CL_SUCCESS;
}

static auto getKernelInfoFields = std::map<cl_kernel_info, std::shared_ptr<Getter<cl_kernel>>>{
	{CL_KERNEL_FUNCTION_NAME, GetField(&_cl_kernel::name)},
	{CL_KERNEL_NUM_ARGS, std::make_shared<GetPrimitiveFromLambda<cl_kernel, cl_uint>>(
		[](cl_kernel kernel) -> cl_uint {
			return kernel->args.size();
		}
	)},
	{CL_KERNEL_CONTEXT, GetField(&_cl_kernel::context)},
	{CL_KERNEL_PROGRAM, GetField(&_cl_kernel::program)},
};

CL_API_ENTRY cl_int CL_API_CALL
clGetKernelInfo(
	cl_kernel kernel,
	cl_kernel_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	if (!verifyKernel(kernel)) return CL_INVALID_PROGRAM;
	auto i = getKernelInfoFields.find(param_name);
	if (i == getKernelInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, kernel);
}

// same as device workgroup size
static size_t clKernelWorkGroupSize = _cl_device_id::maxWorkGroupSize; // which is max compute units, which is concurrency

static auto getKernelWorkGroupInfoFields = std::map<cl_kernel_work_group_info, std::shared_ptr<Getter<cl_kernel>>>{
	{CL_KERNEL_WORK_GROUP_SIZE, GetPrimitiveLiteral<cl_kernel, size_t>(clKernelWorkGroupSize)}
};

CL_API_ENTRY cl_int CL_API_CALL
clGetKernelWorkGroupInfo(
	cl_kernel kernel,
	cl_device_id device,
	cl_kernel_work_group_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	//TODO make use of device? nah.
	if (!verifyKernel(kernel)) return CL_INVALID_PROGRAM;
	auto i = getKernelWorkGroupInfoFields.find(param_name);
	if (i == getKernelWorkGroupInfoFields.end()) return CL_INVALID_VALUE;
	return i->second->getValue(param_value_size, param_value, param_value_size_ret, kernel);
}
