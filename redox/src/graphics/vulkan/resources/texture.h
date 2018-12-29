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
#include "core\non_copyable.h"

#include "graphics\vulkan\vulkan.h"
#include "graphics\vulkan\buffer.h"
#include "graphics\vulkan\sampler.h"

#include "resources/resource.h"

namespace redox::graphics {
	class Texture : public NonCopyable {
	public:
		Texture(VkFormat format, const VkExtent2D& size, 
			VkImageUsageFlags usage, VkImageAspectFlags viewAspectFlags);
		~Texture();

		VkImage handle() const;
		VkImageView view() const;

		const VkExtent2D& dimension() const;
		const VkFormat& format() const;
		const Sampler& sampler() const;
	
	protected:
		void _destroy();
		void _init();
		void _init_view();
		void _transfer_layout(VkImageLayout oldLayout, VkImageLayout newLayout) const;

		Sampler _sampler;
		VkImage _handle;
		VkImageView _view;

		VkImageAspectFlags _viewAspectFlags;
		VkImageUsageFlags _usageFlags;
		VkDeviceMemory _memory;
		VkFormat _format;
		VkExtent2D _dimensions;
	};

	class ResizableTexture : public Texture {
	public:
		using Texture::Texture;
		void resize(const VkExtent2D& extent);
	};

	class DepthTexture : public ResizableTexture {
	public:
		DepthTexture(const VkExtent2D& size);
		void resize(const VkExtent2D& extent);
	};

	class StagedTexture : public IResource, public Texture {
	public:
		StagedTexture(const redox::Buffer<byte>& pixels, VkFormat format,
			const VkExtent2D& size, VkImageUsageFlags usage, VkImageAspectFlags viewAspectFlags);

		~StagedTexture() override = default;

		void map(FunctionRef<void(void*)> fn) const;
		void upload() override;

	protected:
		Buffer _stagingBuffer;
	};

	class SampleTexture : public StagedTexture {
	public:
		SampleTexture(const redox::Buffer<byte>& pixels, VkFormat format, const VkExtent2D& size);
	};

}