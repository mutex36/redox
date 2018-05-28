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
	_renderPass(_graphics),
	_swapchain(_graphics, _renderPass, std::bind(&RenderSystem::_swapchain_event_create, this)),
	_configRef(config),
	_mvpBuffer(sizeof(mvp_uniform), _graphics),
	_demoVs(_shaderFactory.load(RDX_ASSET("shader\\vert.spv"), _graphics)),
	_demoFs(_shaderFactory.load(RDX_ASSET("shader\\frag.spv"), _graphics)),
	_demoMesh(_meshFactory.load(RDX_ASSET("meshes\\box.gltf"), _graphics)),
	_demoTexture(_textureFactory.load("textures\\uvchecker.jpg", _graphics)),
	_pipeline(_graphics, _renderPass, get_layout<MeshVertex>(), _demoVs, _demoFs),
	_auxCommandPool(_graphics, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) {

	_auxCommandPool.quick_submit([this](const CommandBuffer& cbo){
		_demoMesh->upload(cbo);
		_demoTexture->upload(cbo);
	});
}

redox::graphics::RenderSystem::~RenderSystem() {
	_graphics.wait_pending();
}

void redox::graphics::RenderSystem::_demo_setup() {
	_RDX_PROFILE;

	_pipeline.bind_resource(_mvpBuffer, 0);
	_pipeline.bind_resource(*_demoTexture, 1);

	_swapchain.visit([this](const Framebuffer& frameBuffer, const CommandBuffer& commandBuffer) {
		commandBuffer.record([this, &commandBuffer, &frameBuffer]() {
			_pipeline.bind(commandBuffer, frameBuffer);

			IndexedDraw drawCmd{ _demoMesh };
			commandBuffer.submit(drawCmd);

			_pipeline.unbind(commandBuffer);
		});
	});
}

void redox::graphics::RenderSystem::render() {

	_mvpBuffer.map([this](void* data) {
		auto extent = _swapchain.extent();
		auto ratio = static_cast<f32>(extent.width) / static_cast<f32>(extent.height);

		auto bf = reinterpret_cast<mvp_uniform*>(data);
		bf->model = math::Mat44f::identity();
		bf->projection = math::Mat44f::perspective(45.0f, ratio, 0.1f, 10.f);
		bf->view = math::Mat44f::translate({0,0,-2});
	});

	_auxCommandPool.quick_submit([this](const CommandBuffer& cbo) {
		//update buffers
		_mvpBuffer.upload(cbo);
	});

	_swapchain.present();
}

redox::graphics::ShaderFactory& redox::graphics::RenderSystem::shader_factory() {
	return _shaderFactory;
}

redox::graphics::MeshFactory& redox::graphics::RenderSystem::mesh_factory() {
	return _meshFactory;
}

void redox::graphics::RenderSystem::_swapchain_event_create() {
	_pipeline.set_viewport(_swapchain.extent());
	_demo_setup();
}
