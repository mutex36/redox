#include "texture.h"
#include "graphics.h"
#include "command_pool.h"

redox::graphics::Texture::Texture(const redox::Buffer<byte>& pixels, VkFormat format, 
	const Dimension& size, const Graphics& graphics) :
	_graphicsRef(graphics),
	_format(format),
	_dimensions(size),
	_imageSize(pixels.byte_size()),
	_stagingBuffer(_imageSize, graphics, VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
	
	_stagingBuffer.map([&pixels](void* data) {
		std::memcpy(data, pixels.data(), pixels.byte_size());
	});

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
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	if (vkCreateImage(_graphicsRef.device(), &imageInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create image");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(_graphicsRef.device(), _handle, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	auto memType = _graphicsRef.pick_memory_type(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (!memType)
		throw Exception("could not find suitable memory type");
	allocInfo.memoryTypeIndex = memType.value();

	if (vkAllocateMemory(_graphicsRef.device(), &allocInfo, nullptr, &_memory) != VK_SUCCESS)
		throw Exception("failed to allocate image memory");

	vkBindImageMemory(_graphicsRef.device(), _handle, _memory, 0);

	_init_view();
	_init_sampler();
}

redox::graphics::Texture::~Texture() {
	vkDestroyImage(_graphicsRef.device(), _handle, nullptr);
	vkFreeMemory(_graphicsRef.device(), _memory, nullptr);
	vkDestroySampler(_graphicsRef.device(), _sampler, nullptr);
	vkDestroyImageView(_graphicsRef.device(), _view, nullptr);
}

VkImage redox::graphics::Texture::handle() const {
	return _handle;
}

VkSampler redox::graphics::Texture::sampler() const {
	return _sampler;
}

VkImageView redox::graphics::Texture::view() const {
	return _view;
}

const redox::graphics::Texture::Dimension& redox::graphics::Texture::dimension() const {
	return _dimensions;
}

void redox::graphics::Texture::upload(const CommandBuffer& commandBuffer) {
	_transfer_layout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBuffer);
	_transfer_memory(commandBuffer);
	_transfer_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandBuffer);
}

void redox::graphics::Texture::_init_view() {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = _handle;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = _format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(_graphicsRef.device(), &viewInfo, nullptr, &_view) != VK_SUCCESS)
		throw Exception("failed to create texture image view");
}

void redox::graphics::Texture::_init_sampler() {

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 16;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	
	if (vkCreateSampler(_graphicsRef.device(), &samplerInfo, nullptr, &_sampler) != VK_SUCCESS)
		throw Exception("failed to create texture sampler");
}

void redox::graphics::Texture::_transfer_layout(VkImageLayout oldLayout, VkImageLayout newLayout, const CommandBuffer& commandBuffer) {
	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = _handle;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

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
	else throw Exception("unsupported layout transition");

	vkCmdPipelineBarrier(commandBuffer.handle(), sourceStage, destinationStage,
		0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void redox::graphics::Texture::_transfer_memory(const CommandBuffer& commandBuffer) {
	_stagingBuffer.copy_to(*this, commandBuffer);
}