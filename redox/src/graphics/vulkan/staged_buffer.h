#pragma once
#include "vulkan.h"
#include "buffer.h"

namespace redox::graphics {
	class Graphics;
	class CommandBuffer;
	
	class StagedBuffer {
	public:
		StagedBuffer(VkDeviceSize size, const Graphics& graphics, VkBufferUsageFlags usage);
		~StagedBuffer() = default;

		template<class Fn>
		void map(Fn&& fn) {
			_stagingBuffer.map(std::forward<Fn>(fn));
		}

		void upload(const CommandBuffer& commandBuffer);

		const Buffer& main_buffer() const;
		const Buffer& staging_buffer() const;

	private:
		Buffer _buffer;
		Buffer _stagingBuffer;
	};
}