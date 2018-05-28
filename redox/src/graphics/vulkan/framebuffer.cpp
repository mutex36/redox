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
#include "framebuffer.h"
#include "graphics.h"
#include "render_pass.h"

redox::graphics::Framebuffer::Framebuffer(const Graphics& graphics, const RenderPass& rp, VkImageView imageView, VkExtent2D extent) :
_graphicsRef(graphics), _extent(extent) {

	VkFramebufferCreateInfo framebufferInfo{};
	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferInfo.renderPass = rp.handle();
	framebufferInfo.attachmentCount = 1;
	framebufferInfo.pAttachments = &imageView;
	framebufferInfo.width = extent.width;
	framebufferInfo.height = extent.height;
	framebufferInfo.layers = 1;

	if (vkCreateFramebuffer(_graphicsRef.device(), &framebufferInfo, nullptr, &_handle) != VK_SUCCESS)
		throw Exception("failed to create frambuffer");
}

redox::graphics::Framebuffer::~Framebuffer() {
	vkDestroyFramebuffer(_graphicsRef.device(), _handle, nullptr);
}

redox::graphics::Framebuffer::Framebuffer(Framebuffer&& ref) :
	_handle(ref._handle),
	_extent(ref._extent),
	_graphicsRef(ref._graphicsRef) {

}

VkFramebuffer redox::graphics::Framebuffer::handle() const {
	return _handle;
}

const VkExtent2D & redox::graphics::Framebuffer::extent() const {
	return _extent;
}
