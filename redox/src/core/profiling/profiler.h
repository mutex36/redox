#pragma once
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS 
#include "profiler_win.hpp"
#endif

#define _RDX_PROFILE detail::Profiler __profiler(__FUNCTION__);