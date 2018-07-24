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

redox::graphics::RenderSystem::RenderSystem(const platform::Window& window) :
	_graphics(window),
	_swapchain(std::bind(&RenderSystem::_swapchain_event_create, this)),
	_mvpBuffer(sizeof(mvp_uniform)),
	_demoModel(Application::instance->
		resource_manager().load<Model>("meshes\\centurion.gltf")) {

}

redox::graphics::RenderSystem::~RenderSystem() {
	_graphics.wait_pending();
}

void redox::graphics::RenderSystem::_demo_draw() {

	_swapchain.visit([this](const Framebuffer& frameBuffer, const CommandBufferView& commandBuffer) {
		commandBuffer.record([this, &commandBuffer, &frameBuffer]() {
			const auto& fwdPass = Graphics::instance->forward_render_pass();
			fwdPass.begin(frameBuffer, commandBuffer);

			auto mesh = _demoModel->meshes()[0];
			for (const auto& sm : mesh->submeshes()) {
				IndexedDraw drawCmd;
				drawCmd.mesh = mesh;
				drawCmd.material = _demoModel->materials()[sm.materialIndex];
				drawCmd.range.start = sm.vertexOffset;
				drawCmd.range.end = sm.vertexCount;

				commandBuffer.submit(drawCmd);
			}

			fwdPass.end(commandBuffer);
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
	
	_mvpBuffer.upload();
	_swapchain.present();
}

void redox::graphics::RenderSystem::_swapchain_event_create() {

	//for (auto& pipelineIt : Graphics::instance->pipeline_cache())
	//	pipelineIt.value->set_viewport(_swapchain.extent());
	//
	const auto& rp = Graphics::instance->forward_render_pass(); 
	//rp.resize_attachments(_swapchain.extent());
	_swapchain.create_fbs(rp);
	_demo_draw();
}
