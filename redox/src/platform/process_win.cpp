#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "process.h"
#include <platform/windows.h>

struct redox::platform::Process::internal {
	HANDLE pipeReadHandle, pipeWriteHandle;
	PROCESS_INFORMATION info;
};

redox::platform::Process::Process(const String& command) :
	_internal(make_unique<internal>()) {

	SECURITY_ATTRIBUTES saAttr{ sizeof(SECURITY_ATTRIBUTES) };
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&_internal->pipeReadHandle, &_internal->pipeWriteHandle, &saAttr, 0)) {
		throw Exception("CreatePipe() failed.");
	}

	STARTUPINFO si{ sizeof(STARTUPINFO) };
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = _internal->pipeWriteHandle;
	si.hStdError = _internal->pipeWriteHandle;
	si.wShowWindow = SW_HIDE;

	if (!CreateProcess(NULL, const_cast<LPSTR>(command.c_str()), NULL, NULL,
		TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &_internal->info)) {

		throw Exception("CreateProcessW() failed.");
	}
}

redox::platform::Process::~Process() {
	CloseHandle(_internal->pipeWriteHandle);
	CloseHandle(_internal->pipeReadHandle);
	CloseHandle(_internal->info.hProcess);
	CloseHandle(_internal->info.hThread);
}

redox::platform::ProcessOutput redox::platform::Process::join() {
	ProcessOutput output{};
	WaitForSingleObject(_internal->info.hProcess, INFINITE);

	for (;;) {
		i8 buffer[1024];
		DWORD dwRead{ 0 };
		DWORD dwAvail{ 0 };

		if (!PeekNamedPipe(_internal->pipeReadHandle, NULL, 0, NULL, &dwAvail, NULL) || !dwAvail
			|| !ReadFile(_internal->pipeReadHandle, buffer, 
				std::min(sizeof(buffer) - 1, static_cast<std::size_t>(dwAvail)),
				&dwRead, NULL) || !dwRead) {

			break;
		}

		buffer[dwRead] = 0;
		output.stdOut += buffer;
	}

	DWORD exitCode;
	if (!GetExitCodeProcess(_internal->info.hProcess, &exitCode)) {
		throw Exception("GetExitCodeProcess() failed.");
	}

	output.errorCode = static_cast<i32>(exitCode);
	return output;
}



#endif