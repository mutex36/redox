#include "shader_compiler.h"
#include <core/application.h>
#include <platform/filesystem.h>

redox::graphics::ShaderCompiler::ShaderCompiler(String compilerPath, String outputLocation)
	: _compilerPath(std::move(compilerPath)), _outputLocation(std::move(outputLocation)) {
}

redox::String redox::graphics::ShaderCompiler::compile(const String& source) const {
	std::hash<String> hash;
	auto outputFile = _outputLocation + std::to_string(hash(source)) + ".spv";

	io::File of(outputFile);
	if (!of.is_valid()) {
		RDX_LOG("Compiling shader...");

		auto args = redox::format("{0} -V -o {1} {2}", _compilerPath, outputFile, source);
		auto exitCode = std::system(args.c_str());

		if (exitCode != 0) {
			throw Exception("failed to invoke shader compiler.");
		}
	}

	return outputFile;
}
