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
#pragma once
#include "core/string.h"
#include "core/non_copyable.h"
#include "core/buffer.h"

#include "graphics\vulkan\descriptor_pool.h"
#include "graphics\vulkan\vulkan.h"
#include "graphics\vulkan\pipeline_cache.h"
#include "shader.h"

namespace redox::graphics {
	class CommandBufferView;

	enum class TextureKeys {
		ALBEDO, ROUGHNESS_METALNESS, NORMAL, DISPLACEMENT, LIGHT, OCCLUSION, INVALID
	};

	enum class BufferKeys {
		MVP, USER0, USER1
	};

	class Material : public IResource {
	public:
		Material(PipelineHandle pipeline, DescriptorSetView descSet);

		void bind(const CommandBufferView& commandBuffer);
		void upload() override;

		void set_buffer(BufferKeys key, const UniformBuffer& buffer);
		void set_texture(TextureKeys key, ResourceHandle<SampleTexture> texture);

	private:
		DescriptorSetView _descSet;
		PipelineHandle _pipeline;

		redox::Hashmap<TextureKeys, ResourceHandle<SampleTexture>> _textures;
	};
}