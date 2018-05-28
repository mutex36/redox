#pragma once
#include "core\ref_counted.h"
#include "core\string.h"

static const redox::String
	RDX_RESOURCE_PATH(R"(C:\Users\luis9\Desktop\redox\redox\resources\)");

static const redox::String
	RDX_CONFIG_PATH(R"(C:\Users\luis9\Desktop\redox\redox\config\)");

#define RDX_ASSET(file) RDX_RESOURCE_PATH + file
#define RDX_CONFIG_ASSET(file) RDX_CONFIG_PATH + file

namespace redox {
	
	//TODO; might become more complex in the future

	template<class T>
	using Resource = RefCounted<T>;

}