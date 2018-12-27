#pragma once
#include <core/core.h>

namespace redox::graphics {
	class ShaderCompiler {
	public:
		ShaderCompiler(String compilerPath, String outputLocation);
		String compile(const String& source) const;

	private:
		String _compilerPath;
		String _outputLocation;
	};
}