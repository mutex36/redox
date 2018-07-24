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
#include "descriptor_pool.h"
#include "command_pool.h"
#include "graphics.h"
#include "resources/texture.h"
#include "buffer.h"
#include "pipeline.h"

redox::graphics::DescriptorPool::DescriptorPool(uint32_t maxSets, uint32_t maxImages, uint32_t maxUBOs) {

	VkDescriptorPoolSize poolSizes[] = {
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxUBOs },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxImages }
	};

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = util::array_size<uint32_t>(poolSizes);
	poolInfo.pPoolSizes = poolSizes;
	poolInfo.maxSets = maxSets;

	if (vkCreateDescriptorPool(Graphics::instance->device(), &poolInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create descriptor pool");
}

redox::graphics::DescriptorPool::~DescriptorPool() {
	vkDestroyDescriptorPool(Graphics::instance->device(), _handle, nullptr);
}

redox::graphics::DescriptorSetView redox::graphics::DescriptorPool::allocate(VkDescriptorSetLayout layout) const {
	VkDescriptorSet set;
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _handle;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout;

	if (vkAllocateDescriptorSets(Graphics::instance->device(), &allocInfo, &set) != VK_SUCCESS)
		throw Exception("failed to allocate descriptor set");

	return set;
}

redox::graphics::DescriptorSetView::DescriptorSetView(VkDescriptorSet handle) : _handle(handle) {
}

void redox::graphics::DescriptorSetView::bind(const CommandBufferView& commandBuffer, const Pipeline& pipeline) {
	vkCmdBindDescriptorSets(commandBuffer.handle(),
		VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.layout(), 0, 1, &_handle, 0, nullptr);
}

void redox::graphics::DescriptorSetView::bind_resource(const Texture& texture, uint32_t bindingPoint) {

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture.view();
	imageInfo.sampler = texture.sampler().handle();

	VkWriteDescriptorSet writeSet{};
	writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeSet.dstSet = _handle;
	writeSet.dstBinding = bindingPoint;
	writeSet.dstArrayElement = 0;
	writeSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writeSet.descriptorCount = 1;
	writeSet.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(Graphics::instance->device(), 1, &writeSet, 0, nullptr);
}

void redox::graphics::DescriptorSetView::bind_resource(const UniformBuffer& ubo, uint32_t bindingPoint) {

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

	vkUpdateDescriptorSets(Graphics::instance->device(), 1, &writeSet, 0, nullptr);
}
