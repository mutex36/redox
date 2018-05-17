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
#include "render_system.h"

#include "core\buffer.h"
#include "core\string.h"
#include "core\utility.h"

#include "core\profiling\profiler.h"

#define RDX_LOG_TAG "RenderSystem"

redox::graphics::RenderSystem::RenderSystem(const platform::Window& window, const Configuration& config) :
	_graphics(window, config),
	_swapchain(_graphics, std::bind(&RenderSystem::_swapchain_event_recreate, this)),
	_configRef(config),
	_mvpBuffer(sizeof(mvp_uniform), _graphics, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, false),
	_demoVs(_shaderFactory.load("shader\\vert.spv", _graphics)),
	_demoFs(_shaderFactory.load("shader\\frag.spv", _graphics)),
	_demoMesh(_meshFactory.load("meshes\\test.rdxmesh", _graphics, _auxCommandPool)),
	_pipeline(_graphics, _demoVs, _demoFs),
	_auxCommandPool(_graphics, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) {

}

redox::graphics::RenderSystem::~RenderSystem() {
	_wait_pending();
}

void redox::graphics::RenderSystem::demo_setup() {
	_RDX_PROFILE;

	_mvpBuffer.map([](void* data) {
		auto bf = reinterpret_cast<mvp_uniform*>(data);
		bf->model = math::Mat44f::identity();
		bf->projection = math::Mat44f::identity();
		bf->view = math::Mat44f::identity();
	});
	_pipeline.register_ubo(_mvpBuffer);

	_swapchain.visit([this](const Framebuffer& frameBuffer, VkCommandBuffer commandBuffer) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw Exception("failed to begin recording of commandBuffer");

		_pipeline.bind(commandBuffer, frameBuffer);
		_demoMesh->bind(commandBuffer);
		vkCmdDrawIndexed(commandBuffer, _demoMesh->instance_count(), 1, 0, 0, 0);
		_pipeline.unbind(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw Exception("failed to end recording of commandBuffer");
	});
}

void redox::graphics::RenderSystem::render() {
	_swapchain.present();
}

void redox::graphics::RenderSystem::_swapchain_event_recreate() {
	RDX_LOG("recreating swapchain...");

	_pipeline.set_viewport(_swapchain.extent(), _auxCommandPool);
}

void redox::graphics::RenderSystem::_wait_pending() {
	vkDeviceWaitIdle(_graphics.device());
}

redox::graphics::ShaderFactory & redox::graphics::RenderSystem::shader_factory() {
	return _shaderFactory;
}

redox::graphics::MeshFactory & redox::graphics::RenderSystem::mesh_factory() {
	return _meshFactory;
}