#pragma once
#include "vulkan.h"
#include "buffer.h"
#include "core\smart_ptr.h"
#include "core\non_copyable.h"

#include "core\buffer.h"

namespace redox::graphics {
	class Graphics;
	class CommandBuffer;

	class Texture : public NonCopyable {
	public:
		struct Dimension {
			uint32_t width, height;
		};

		VkImage handle() const;
		VkSampler sampler() const;

		const Dimension& dimension() const;
		void upload(const CommandBuffer& commandBuffer);


		Texture(const redox::Buffer<byte>& pixels, VkFormat format,
			const Dimension& size, const Graphics& graphics);
		~Texture();

	private:
		void _init_view();
		void _init_sampler();

		void _transfer_layout(VkImageLayout oldLayout, VkImageLayout newLayout, 
			const CommandBuffer& commandBuffer);
		void _transfer_memory(const CommandBuffer& commandBuffer);

		VkImage _handle;
		VkImageView _view;
		VkSampler _sampler;

		VkDeviceMemory _memory;
		VkFormat _format;
		Dimension _dimensions;
		VkDeviceSize _imageSize;

		Buffer _stagingBuffer;
		const Graphics& _graphicsRef;
	};
}