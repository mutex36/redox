#pragma once
#include "vulkan.h"

namespace redox::graphics {
	class Graphics;
	class CommandBuffer;
	class Texture;
	class UniformBuffer;

	class DescriptorSet : public NonCopyable {
	public:
		DescriptorSet(VkDescriptorSet handle, const Graphics& graphics);

		void bind(const CommandBuffer& commandBuffer);

		void bind_resource(const Texture& texture, uint32_t bindingPoint);
		void bind_resource(const UniformBuffer& ubo, uint32_t bindingPoint);

	private:
		VkDescriptorSet _handle;
		const Graphics& _graphicsRef;
	};

	class DescriptorPool : public NonCopyable {
	public:
		DescriptorPool(const Graphics& graphics);
		~DescriptorPool();

		void allocate(uint32_t numSets);
		DescriptorSet operator[](std::size_t index) const;
		
	private:
		redox::Buffer<VkDescriptorSet> _sets;

		VkDescriptorPool _handle;
		const Graphics& _graphicsRef;
	};
}