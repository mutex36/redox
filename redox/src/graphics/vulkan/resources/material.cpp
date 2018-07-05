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
#include "material.h"
#include "graphics\vulkan\graphics.h"

redox::graphics::Material::Material(PipelineHandle pipeline, const DescriptorSet& descSet) :
	_pipeline(std::move(pipeline)),
	_descSet(descSet),
	_textures(TextureKeys::INVALID) {
}

void redox::graphics::Material::bind(const CommandBuffer& commandBuffer) {
	_pipeline->bind(commandBuffer);
	_descSet.bind(commandBuffer, *_pipeline);
}

void redox::graphics::Material::upload(const CommandBuffer& commandBuffer) {
	_albedoTexture->upload(commandBuffer);
}

void redox::graphics::Material::set_buffer(BufferKeys key, const UniformBuffer& buffer) {

	switch (key) {
	case redox::graphics::BufferKeys::MVP:
		_descSet.bind_resource(buffer, 0);
		break;
	}
}

void redox::graphics::Material::set_texture(TextureKeys key, Resource<SampleTexture> texture) {

	switch (key) {
	case redox::graphics::TextureKeys::ALBEDO:
		_descSet.bind_resource(*texture, 1);
		break;
	}

	_textures.push(key, std::move(texture));
}
