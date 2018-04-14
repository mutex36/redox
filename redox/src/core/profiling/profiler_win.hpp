#pragma once
#include "profiler.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "core\core.h"
#include "core\sys\windows.h"
#include "core\string.h"
#include "core\logging\log.h"

namespace detail {
	struct Profiler {
		_RDX_INLINE Profiler(const redox::String& name) {
			RDX_LOG("PROFILER: {0}", name);
			QueryPerformanceCounter(&_freq);
			QueryPerformanceCounter(&_start);
		}

		_RDX_INLINE ~Profiler() {
			QueryPerformanceCounter(&_end);

			auto freq = static_cast<redox::f64>(_freq.QuadPart);
			auto diff = static_cast<redox::f64>(_end.QuadPart - _start.QuadPart);
			auto result = diff * 1000.0 / freq;

			RDX_LOG("PROFILER: {0}ms", result);
		}

		LARGE_INTEGER _start;
		LARGE_INTEGER _end;
		LARGE_INTEGER _freq;
	};
}

#endif