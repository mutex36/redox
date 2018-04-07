#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "log.h"
#include <Windows.h>

struct redox::Log::PIMPL {
	HANDLE stdout_handle;
};

redox::Log::Log(const DynamicString& name) : _name(name) {
	_pimpl = make_smart_ptr<PIMPL>();
	_pimpl->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
}

redox::Log::~Log() {
	CloseHandle(_pimpl->stdout_handle);
}

redox::Log& redox::Log::operator<<(const char * str) {
	WriteConsole(_pimpl->stdout_handle, str, std::strlen(str), NULL, NULL);
	return *this;
}

redox::Log& redox::Log::operator<<(const DynamicString & str) {
	WriteConsole(_pimpl->stdout_handle, str.cstr(), str.size(), NULL, NULL);
	return *this;
}

#endif