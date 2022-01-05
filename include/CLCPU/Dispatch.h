#pragma once

#include <CL/cl.h>

// why does cl_icd.h need this defined?
// not sure whether to define it in buildinfo or here:
#define CL_API_SUFFIX__VERSION_1_0_DEPRECATED
#define CL_API_SUFFIX__VERSION_1_1_DEPRECATED

#include "cl_icd.h"

extern cl_icd_dispatch dispatchTable;
