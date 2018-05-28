#pragma once
#include "vulkan.h"
#include "buffer.h"
#include "graphics.h"

namespace redox::graphics {
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
		VkBuffer handle() const;

	private:
		Buffer _buffer;
		Buffer _stagingBuffer;
	};

	struct UniformBuffer : public StagedBuffer {
		UniformBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
		}
	};

	struct VertexBuffer : public StagedBuffer {
		VertexBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
		}
	};

	struct IndexBuffer : public StagedBuffer {
		IndexBuffer(VkDeviceSize size, const Graphics& graphics) :
			StagedBuffer(size, graphics, VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
		}
	};
}