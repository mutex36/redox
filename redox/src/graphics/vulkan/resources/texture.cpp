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
#include "texture.h"
#include "graphics\vulkan\graphics.h"
#include "graphics\vulkan\render_system.h"
#include "graphics\vulkan\command_pool.h"

redox::graphics::Texture::Texture(VkFormat format, const VkExtent2D& size,
	VkImageUsageFlags usage, VkImageAspectFlags viewAspectFlags) :
	_format(format),
	_dimensions(size),
	_usageFlags(usage),
	_viewAspectFlags(viewAspectFlags) {

	_init();
	_init_view();
}

redox::graphics::Texture::~Texture() {
	_destroy();
}

void redox::graphics::Texture::_destroy() {
	vkDestroyImage(Graphics::instance().device(), _handle, nullptr);
	vkFreeMemory(Graphics::instance().device(), _memory, nullptr);
	vkDestroyImageView(Graphics::instance().device(), _view, nullptr);
}

VkImage redox::graphics::Texture::handle() const {
	return _handle;
}

VkImageView redox::graphics::Texture::view() const {
	return _view;
}

const VkExtent2D& redox::graphics::Texture::dimension() const {
	return _dimensions;
}

const VkFormat& redox::graphics::Texture::format() const {
	return _format;
}

const redox::graphics::Sampler& redox::graphics::Texture::sampler() const {
	return _sampler;
}

void redox::graphics::Texture::_init() {

	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = _dimensions.width;
	imageInfo.extent.height = _dimensions.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = _format;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = _usageFlags; //VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(Graphics::instance().device(), &imageInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(Graphics::instance().device(), _handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	auto memType = Graphics::instance().pick_memory_type(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (!memType)
		throw Exception("could not find suitable memory type");
	allocInfo.memoryTypeIndex = memType.value();

	if (vkAllocateMemory(Graphics::instance().device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
		throw Exception("failed to allocate image memory");

	vkBindImageMemory(Graphics::instance().device(), _handle, _memory, 0);
}

void redox::graphics::Texture::_init_view() {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _handle;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = _viewAspectFlags; //VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(Graphics::instance().device(), &viewInfo, nullptr, &_view) != VK_SUCCESS)
		throw Exception("failed to create texture image view");
}

void redox::graphics::Texture::_transfer_layout(VkImageLayout oldLayout, VkImageLayout newLayout) const {
	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = _handle;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	else
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else throw Exception("unsupported layout transition");

	RenderSystem::instance().aux_command_pool().quick_submit(
	[sourceStage, destinationStage, &barrier](CommandBufferView cbo) {
		vkCmdPipelineBarrier(cbo.handle(), sourceStage, destinationStage,
			0, 0, nullptr, 0, nullptr, 1, &barrier);
	});
}

redox::graphics::StagedTexture::StagedTexture(const redox::Buffer<byte>& pixels, VkFormat format, const VkExtent2D& size,
	VkImageUsageFlags usage, VkImageAspectFlags viewAspectFlags) :
	Texture(format, size, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT, viewAspectFlags),
	_stagingBuffer(byte_size(pixels), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) {

	_stagingBuffer.map([&pixels](void* data) {
		std::memcpy(data, pixels.data(), byte_size(pixels));
	});
}

void redox::graphics::StagedTexture::upload() {
	_transfer_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	_stagingBuffer.copy_to(*this);
	_transfer_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

redox::graphics::SampleTexture::SampleTexture(const redox::Buffer<byte>& pixels, VkFormat format, const VkExtent2D & size) :
	StagedTexture(pixels, format, size, VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT) {
}

void redox::graphics::ResizableTexture::resize(const VkExtent2D& extent) {
	_dimensions = extent;
	_destroy();
	_init();
	_init_view();
}

redox::graphics::DepthTexture::DepthTexture(const VkExtent2D& size) :
	ResizableTexture(VK_FORMAT_D32_SFLOAT, size, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT) {

	_transfer_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void redox::graphics::DepthTexture::resize(const VkExtent2D& extent) {
	ResizableTexture::resize(extent);
	_transfer_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}