#include "descriptor_pool.h"
#include "command_pool.h"

#include "graphics.h"
#include "resources/texture.h"
#include "buffer.h"

redox::graphics::DescriptorPool::DescriptorPool(const Graphics& graphics) :
	_graphicsRef(graphics) {

	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 20 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 20 }
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = util::array_size<uint32_t>(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = 5;

	if (vkCreateDescriptorPool(_graphicsRef.device(), &poolInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create descriptor pool");
}

redox::graphics::DescriptorPool::~DescriptorPool() {
	vkDestroyDescriptorPool(_graphicsRef.device(), _handle, nullptr);
}

void redox::graphics::DescriptorPool::allocate(uint32_t numSets) {
	_sets.resize(numSets);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _handle;
	allocInfo.descriptorSetCount = numSets;
	allocInfo.pSetLayouts = &_descriptorSetLayout;

	if (vkAllocateDescriptorSets(_graphicsRef.device(), &allocInfo, _sets.data()) != VK_SUCCESS)
		throw Exception("failed to allocate descriptor set");
}

redox::graphics::DescriptorSet redox::graphics::DescriptorPool::operator[](std::size_t index) const {
	return { _sets[index], _graphicsRef };
}

redox::graphics::DescriptorSet::DescriptorSet(VkDescriptorSet handle, const Graphics& graphics) :
	_handle(handle),
	_graphicsRef(graphics) {

}

void redox::graphics::DescriptorSet::bind(const CommandBuffer& commandBuffer) {
	vkCmdBindDescriptorSets(commandBuffer.handle(),
		VK_PIPELINE_BIND_POINT_GRAPHICS, lay, 0, 1, &_handle, 0, nullptr);
}

void redox::graphics::DescriptorSet::bind_resource(const Texture& texture, uint32_t bindingPoint) {

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.view();
	imageInfo.sampler = _defaultSampler.handle();

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.dstSet = _handle;
	writeSet.dstBinding = bindingPoint;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeSet.descriptorCount = 1;
	writeSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(_graphicsRef.device(), 1, &writeSet, 0, nullptr);
}

void redox::graphics::DescriptorSet::bind_resource(const UniformBuffer& ubo, uint32_t bindingPoint) {

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = ubo.handle();
	bufferInfo.offset = 0;
	bufferInfo.range = VK_WHOLE_SIZE;

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.dstSet = _handle;
	writeSet.dstBinding = bindingPoint;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSet.descriptorCount = 1;
	writeSet.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(_graphicsRef.device(), 1, &writeSet, 0, nullptr);
}
