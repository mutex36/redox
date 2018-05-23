#pragma once
#include "vulkan.h"
#include "command_buffer.h"
#include "framebuffer.h"
#include "core\non_copyable.h"

namespace redox::graphics {
	class Graphics;

	class RenderPass : public NonCopyable {
	public:
		RenderPass(const Graphics& graphicsRef);
		~RenderPass();

		void begin(const Framebuffer& frameBuffer, 
			const CommandBuffer& commandBuffer) const;
		void end(const CommandBuffer& commandBuffer) const;
		VkRenderPass handle() const;

	private:
		const Graphics& _graphicsRef;
		VkRenderPass _handle;
	};
}