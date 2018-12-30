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
#include "shader.h"
#include "graphics\vulkan\graphics.h"

#include "platform\filesystem.h"

redox::graphics::Shader::Shader(const redox::Buffer<i8>& buffer) {

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = buffer.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

	if (vkCreateShaderModule(Graphics::instance().device(), &createInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create shader module");
}

redox::graphics::Shader::~Shader() {
	vkDestroyShaderModule(Graphics::instance().device(), _handle, nullptr);
}

VkShaderModule redox::graphics::Shader::handle() const {
	return _handle;
}

void redox::graphics::Shader::upload() {
	//SPIR-V compile...?
}

redox::ResourceGroup redox::graphics::Shader::res_group() const {
	return ResourceGroup::GRAPHICS;
}
