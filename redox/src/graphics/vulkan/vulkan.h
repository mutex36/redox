/*
redox
-----------
MIT License

Copyright (c) 2018 Luis von der Eltz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <core\logging\log.h>
#include <vulkan/vulkan.h>

#ifdef RDX_PLATFORM_WINDOWS 
#include "platform\windows.h"
#include <vulkan\vulkan_win32.h>
#endif

#ifdef RDX_DEBUG
#define RDX_VULKAN_VALIDATION
#endif

#define RDX_VULKAN_MAX_DESC_SETS 5
#define RDX_VULKAN_MAX_DESC_SAMPLERS 20
#define RDX_VULKAN_MAX_DESC_UBOS 5

static constexpr const char* RDX_VULKAN_LAYERS[] = {
	"VK_LAYER_LUNARG_standard_validation"
};

static constexpr const char* RDX_VULKAN_EXTENSIONS[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,

#ifdef RDX_PLATFORM_WINDOWS
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif

#ifdef RDX_VULKAN_VALIDATION
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
};

static constexpr const char* RDX_VULKAN_DEVICE_EXTENSIONS[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef RDX_VULKAN_VALIDATION
static VkBool32 DebugMessageCallback(
	VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
	size_t location, int32_t msgCode, const char* pLayerPrefix, const char* pMsg, void* pUserData) {

	RDX_DEBUG_LOG("Debug Message (Layer: {0}, Code: {1}): {2}", pLayerPrefix, msgCode, pMsg);

	RDX_ASSERT_FALSE(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT);
	RDX_ASSERT_FALSE(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT);

	return VK_FALSE;
}
#endif

RDX_INLINE redox::String lexical_cast(const VkResult& result) {
	switch (result) {
	case VK_NOT_READY: return "VK_NOT_READY";
	case VK_TIMEOUT: return "VK_TIMEOUT";
	case VK_EVENT_SET: return "VK_EVENT_SET";
	case VK_EVENT_RESET: return "VK_EVENT_RESET";
	case VK_INCOMPLETE: return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";

	case VK_SUCCESS: return "VK_SUCCESS";
	default: return "UNKNOWN_ERROR";
	}
}

inline bool operator==(const VkExtent2D& a, const VkExtent2D& b) {
	return a.width == b.width && a.height == b.height;
}

inline bool operator!=(const VkExtent2D& a, const VkExtent2D& b) {
	return !(a == b);
}