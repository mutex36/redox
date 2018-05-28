#pragma once
#include "vulkan.h"
#include "mesh.h"
#include "resources/factory.h"
#include "core/non_copyable.h"

namespace redox::graphics {

	struct IndexedDraw {
		Resource<Mesh> mesh;
	};

	class CommandBuffer : public NonCopyable {
	public:
		CommandBuffer(VkCommandBuffer handle);
		~CommandBuffer() = default;

		void submit(const IndexedDraw& command) const {
			command.mesh->bind(_handle);
			vkCmdDrawIndexed(_handle, command.mesh->instance_count(), 1, 0, 0, 0);
		}

		template<class Fn>
		void record(Fn&& fn) const {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if (vkBeginCommandBuffer(_handle, &beginInfo) != VK_SUCCESS)
				throw Exception("failed to begin recording of commandBuffer");

			fn();

			if (vkEndCommandBuffer(_handle) != VK_SUCCESS)
				throw Exception("failed to end recording of commandBuffer");
		}
		
		VkCommandBuffer handle() const;

	private:
		VkCommandBuffer _handle;
	};
}