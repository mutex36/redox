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
#include "shader_compiler.h"
#include <core/application.h>
#include <platform/filesystem.h>
#include <platform/process.h>

redox::Path redox::graphics::ShaderCompiler::compile(const Path& source, const Path& outputFolder, bool overwrite) {
	std::hash<Path> hash;
	auto cacheFile = outputFolder / (redox::lexical_cast(hash(source)) + ".spv");

	if (!io::exists(cacheFile) || overwrite) {
		RDX_LOG("Compiling shader...");

		auto args = redox::format("glslangValidator -o {0} -V {1}", cacheFile, source);
		RDX_DEBUG_LOG("{0}", args);

		platform::Process p(args);
		auto result = p.join();

		if (result.errorCode != 0) {
			RDX_LOG("Failed to compile shader [Exit Code: {0}] \n {1}", ConsoleColor::RED,
				result.errorCode, result.stdOut);
			throw Exception("failed to invoke glslangValidator.");
		}

	}

	return cacheFile;
}
