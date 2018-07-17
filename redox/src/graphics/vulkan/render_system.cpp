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
#include "core\application.h"

#define RDX_LOG_TAG "RenderSystem"

redox::graphics::RenderSystem::RenderSystem(const platform::Window& window, const Configuration& config) :
	_graphics(window, config),
	_swapchain(std::bind(&RenderSystem::_swapchain_event_create, this)),
	_mvpBuffer(sizeof(mvp_uniform)),
	_modelFactory(_pipelineCache, _mvpBuffer, _textureFactory, _descriptorPool),
	_auxCommandPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT),
	_pipelineCache(_renderPass, _shaderFactory),
	_descriptorPool(5, 20, 20),
	_demoModel(_modelFactory.load(RDX_ASSET("meshes\\centurion.gltf"))) {

	_auxCommandPool.quick_submit([this](const CommandBuffer& cbo) {
		_demoModel->upload(cbo);
	});
}

redox::graphics::RenderSystem::~RenderSystem() {
	_graphics.wait_pending();
}

void redox::graphics::RenderSystem::_demo_draw() {

	_swapchain.visit([this](const Framebuffer& frameBuffer, const CommandBuffer& commandBuffer) {
		commandBuffer.record([this, &commandBuffer, &frameBuffer]() {
			_renderPass.begin(frameBuffer, commandBuffer);

			auto mesh = _demoModel->meshes()[0];
			for (const auto& sm : mesh->submeshes()) {
				IndexedDraw drawCmd;
				drawCmd.mesh = mesh;
				drawCmd.material = _demoModel->materials()[sm.materialIndex];
				drawCmd.range.start = sm.vertexOffset;
				drawCmd.range.end = sm.vertexCount;

				commandBuffer.submit(drawCmd);
			}

			_renderPass.end(commandBuffer);
		});
	});

}

void redox::graphics::RenderSystem::render() {

	_mvpBuffer.map([this](void* data) {
		auto extent = _swapchain.extent();
		auto ratio = static_cast<f32>(extent.width) / static_cast<f32>(extent.height);

		static float k = 0.0;
		k += 0.02f;

		auto bf = reinterpret_cast<mvp_uniform*>(data);
		bf->model = math::Mat44f::rotate_y(k);
		bf->projection = math::Mat44f::perspective(45.0f, ratio, 0.1f, 100.f);
		bf->view = math::Mat44f::translate({0,0,-3});
	});

	_auxCommandPool.quick_submit([this](const CommandBuffer& cbo) {
		_mvpBuffer.upload(cbo);
	});

	_swapchain.present();
}

void redox::graphics::RenderSystem::_swapchain_event_create() {

	for (auto& pipelineIt : _pipelineCache)
		pipelineIt.value->set_viewport(_swapchain.extent());
	
	_auxCommandPool.quick_submit([this](const CommandBuffer& cbo) {
		_renderPass.resize_attachments(cbo, _swapchain.extent()); 
	});
	_swapchain.create_fbs(_renderPass);
	_demo_draw();
}
