#pragma once
#include <core/core.h>

namespace redox::platform {

	struct ProcessOutput {
		i32 errorCode;
		String stdOut;
	};

	class Process {
	public:
		Process(const String& command);
		~Process();

		ProcessOutput join();

	private:
		struct internal;
		UniquePtr<internal> _internal;
	};
}