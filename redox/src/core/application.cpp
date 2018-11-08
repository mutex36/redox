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
#include <core/application.h>
#include <core/string_format.h>
#include <platform/misc.h>

redox::Application* redox::Application::instance = nullptr;

redox::Application::Application() :
	_config("config\\settings.ini") {

	platform::WindowSettings wdSettings{};
	wdSettings.iconPath = _config.get("Surface", "icon").as<String>();
	wdSettings.width = _config.get("Surface", "resolution_x");
	wdSettings.height = _config.get("Surface", "resolution_y");

	if (_config.get("Surface", "fullscreen"))
		wdSettings.flags |= platform::WindowFlags::FULLSCREEN;

	if (_config.get("Surface", "resizable"))
		wdSettings.flags |= platform::WindowFlags::RESIZABLE;

	_resourceManager = std::make_unique<ResourceManager>();
	_timer = std::make_unique<platform::Timer>();
	_window = std::make_unique<platform::Window>(wdSettings);
	_renderSystem = std::make_unique<graphics::RenderSystem>(*_window);
	_inputSystem = std::make_unique<input::InputSystem>(*_window);

	_window->set_callback([this](platform::Window::Event event) {
		switch (event) {
		case platform::Window::Event::CLOSE:
			stop(); break;
		case platform::Window::Event::LOSTFOCUS:
			if (_state != State::TERMINATED)
				_state = State::PAUSED;
			break;
		case platform::Window::Event::GAINFOCUS:
			_state = State::RUNNING;
			break;
		}
	});

	RDX_LOG("Initializing Application...", ConsoleColor::GREEN);
}

redox::Application::~Application() {
}

void redox::Application::run() {
	_window->show();
	_state = State::RUNNING;

	const u32 max_fps = _config.get("Engine", "max_fps");
	const auto timestep = 1000. / max_fps;

	_timer->start();

	while (_state != State::TERMINATED) {
		auto dt_ms = _timer->elapsed();

		_window->process_events();
		_inputSystem->poll();

		//if (_state == State::PAUSED) {
		//	RDX_SLEEP_MS(1);
		//	continue;
		//}

		if (dt_ms >= timestep) {
			_timer.reset();
			_renderSystem->render();

			_window->set_title(
				redox::format("redox engine | {0}fps", 1000. / dt_ms));
		}
	}
}

void redox::Application::stop() {
	_state = State::TERMINATED;
	RDX_LOG("Terminating Application...", ConsoleColor::RED);
}

const redox::Configuration* redox::Application::config() const {
	return &_config;
}

const redox::platform::Timer* redox::Application::timer() const {
	return _timer.get();
}

const redox::ResourceManager* redox::Application::resource_manager() const {
	return _resourceManager.get();
}

const redox::graphics::RenderSystem* redox::Application::render_system() const {
	return _renderSystem.get();
}
