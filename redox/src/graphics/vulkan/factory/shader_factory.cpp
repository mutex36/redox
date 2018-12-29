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
#include "shader_factory.h"
#include <resources/resource_manager.h>

#include <algorithm> //std::find

redox::graphics::ShaderFactory::ShaderFactory() : 
	_compiler(io::absolute("assets\\shader\\compiled\\")) {
	_compiler.clear_output();
}

redox::ResourceHandle<redox::IResource> redox::graphics::ShaderFactory::load(const Path& path) {
	auto output = _compiler.compile(path);

	io::File fstream(output, io::File::Mode::READ | io::File::Mode::THROW_IF_INVALID);
	auto buffer = fstream.read();

	return std::make_shared<Shader>(std::move(buffer));
}

void redox::graphics::ShaderFactory::reload(const ResourceHandle<IResource>& resource, const Path& path) {
	//TODO: implement
}

bool redox::graphics::ShaderFactory::supports_ext(const Path& ext) {
	Array<StringView, 4> supported = { ".frag", ".vert", ".geom" };
	return std::find(supported.begin(), supported.end(), ext) != supported.end();
}
