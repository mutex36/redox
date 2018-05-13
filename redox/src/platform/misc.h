#pragma once
#include "core\core.h"

namespace redox::platform {

#ifdef RDX_PLATFORM_WINDOWS
	#include "platform/windows.h"
	#define RDX_SLEEP_MS(s) Sleep(s)
#else
	#include <unistd.h>
	#define RDX_SLEEP_MS(s) sleep(s * 1000)
#endif

}