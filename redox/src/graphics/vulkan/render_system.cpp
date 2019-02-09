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
#include <graphics/vulkan/render_system.h>

#include <core/utility.h>
#include <core/profiling/profiler.h>
#include <core/application.h>

const redox::graphics::RenderSystem* redox::graphics::RenderSystem::instance() {
	return Application::instance->render_system();
}

redox::graphics::RenderSystem::RenderSystem() :
	_mvpBuffer(sizeof(mvp_uniform)),
	//TODO: Set depending on application
	_descriptorPool(100, 100, 100) {

	_swapchain = make_unique<Swapchain>();
	_swapchain->onResize += [this]() {
		_swapchain_event_resize();
	};

	_forwardPass = make_unique<RenderPass>(_swapchain->extent());
	_swapchain->create_fbs(*_forwardPass);

	_pipelineCache = make_unique<PipelineCache>(_forwardPass.get());
	_pipelineCache->onCreate += [this](PipelineHandle pipeline) {
		pipeline->set_viewport(_swapchain->extent());
	};

	_textureFactory = make_unique<TextureFactory>();
	_modelFactory = make_unique<ModelFactory>(&_descriptorPool, _pipelineCache.get());
	_shaderFactory = make_unique<ShaderFactory>();

	ResourceManager::instance()->register_factory(_textureFactory.get());
	ResourceManager::instance()->register_factory(_modelFactory.get());
	ResourceManager::instance()->register_factory(_shaderFactory.get());

	_demo_load_assets();
}

redox::graphics::RenderSystem::~RenderSystem() {
	Graphics::instance().wait_pending();
}

void redox::graphics::RenderSystem::_demo_cam_move() {
	static math::Vec3f camPosition{ 0,0,-30 };

	auto input = Application::instance->input_system();
	if (input->key_state(input::Keys::W) == input::KeyState::PRESSED) {
		camPosition.z += 0.5f;
	}
	else if (input->key_state(input::Keys::S) == input::KeyState::PRESSED) {
		camPosition.z -= 0.5f;
	}

	if (input->key_state(input::Keys::D) == input::KeyState::PRESSED) {
		camPosition.x += 0.5f;
	}
	else if (input->key_state(input::Keys::A) == input::KeyState::PRESSED) {
		camPosition.x -= 0.5f;
	}

	if (input->key_state(input::Keys::Q) == input::KeyState::PRESSED) {
		camPosition.y += 0.5f;
	}
	else if (input->key_state(input::Keys::E) == input::KeyState::PRESSED) {
		camPosition.y -= 0.5f;
	}

	_mvpBuffer.map<mvp_uniform>([this](mvp_uniform* data) {
		auto extent = _swapchain->extent();
		auto ratio = static_cast<f32>(extent.width) / static_cast<f32>(extent.height);

		data->model = math::Mat44f::rotate_euler({ -90, 0, 0 });
		data->projection = math::Mat44f::perspective(45.0f, ratio, 0.1f, 1000.f);
		data->view = math::Mat44f::translate(camPosition);
	});

	_mvpBuffer.upload();
}

void redox::graphics::RenderSystem::_demo_draw() {
	_swapchain->visit([this](const Framebuffer& frameBuffer, const CommandBufferView& commandBuffer) {
		RDX_UNUSED(commandBuffer.scoped_record());
		RDX_UNUSED(_forwardPass->scoped_begin(frameBuffer, commandBuffer));

		for (const auto& mesh : _demoModel->meshes()) {
			for (const auto& sm : mesh->submeshes()) {
				auto material = _demoModel->materials()[sm.materialIndex];
				commandBuffer.submit(IndexedDraw{
					mesh, material, {sm.indexOffset, sm.indexCount}
				});
			}
		}
	});
}

void redox::graphics::RenderSystem::_demo_load_assets() {
	_demoModel = ResourceManager::instance()->load<Model>("meshes\\scene.gltf");

	if (!_demoModel) {
		throw Exception("failed to load model.");
	}

	for (auto& mat : _demoModel->materials()) {
		mat->set_buffer(BufferKeys::MVP, _mvpBuffer);
	}

	_demoModel->upload();
}

void redox::graphics::RenderSystem::render() {
	_demo_cam_move();
	_demo_draw();
	_swapchain->present();
}

void redox::graphics::RenderSystem::_swapchain_event_resize() {
	for (const auto& p : *_pipelineCache) {
		p.second->set_viewport(_swapchain->extent());
	}

	_forwardPass->resize_attachments(_swapchain->extent());
	_swapchain->create_fbs(*_forwardPass);
}