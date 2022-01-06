#include "CLCPU/Sampler.h"

// TODO IMPLEMENETME maybe
// who uses this anyways?

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
clCreateImage2D(
	cl_context context,
	cl_mem_flags flags,
	const cl_image_format * image_format,
	size_t image_width,
	size_t image_height,
	size_t image_row_pitch,
	void * host_ptr,
	cl_int * errcode_ret
) {
	if (errcode_ret) *errcode_ret = CL_INVALID_VALUE; 
	//CL_MEM_OBJECT_IMAGE2D
	return nullptr;
}

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_mem CL_API_CALL
clCreateImage3D(
	cl_context context,
	cl_mem_flags flags,
	const cl_image_format * image_format,
	size_t image_width,
	size_t image_height,
	size_t image_depth,
	size_t image_row_pitch,
	size_t image_slice_pitch,
	void * host_ptr,
	cl_int * errcode_ret
) {
	if (errcode_ret) *errcode_ret = CL_INVALID_VALUE;
	//CL_MEM_OBJECT_IMAGE3D 
	return nullptr;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetSupportedImageFormats(
	cl_context context,
	cl_mem_flags flags,
	cl_mem_object_type image_type,
	cl_uint num_entries,
	cl_image_format * image_formats,
	cl_uint * num_image_formats
) {
	return CL_INVALID_VALUE;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetImageInfo(
	cl_mem image,
	cl_image_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	return CL_INVALID_VALUE;
}

CL_API_ENTRY CL_EXT_PREFIX__VERSION_1_2_DEPRECATED cl_sampler CL_API_CALL
clCreateSampler(
	cl_context context,
	cl_bool normalized_coords,
	cl_addressing_mode addressing_mode,
	cl_filter_mode filter_mode,
	cl_int * errcode_ret
) {
	if (errcode_ret) *errcode_ret = CL_INVALID_VALUE;
	return nullptr;
}

CL_API_ENTRY cl_int CL_API_CALL
clRetainSampler(
	cl_sampler sampler
) {
	return CL_INVALID_VALUE;
}

CL_API_ENTRY cl_int CL_API_CALL
clReleaseSampler(
	cl_sampler sampler
) {
	return CL_INVALID_VALUE;
}

CL_API_ENTRY cl_int CL_API_CALL
clGetSamplerInfo(
	cl_sampler sampler,
	cl_sampler_info param_name,
	size_t param_value_size,
	void * param_value,
	size_t * param_value_size_ret
) {
	return CL_INVALID_VALUE;
}
