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
#include "application.h"

#include "string_format.h"
#include "platform\misc.h"

#define RDX_LOG_TAG "Application"

redox::Application::Application() :
	_config("settings.ini"),
	_window("redox engine"),
	_renderSystem(_window),
	_inputSystem(_window),
	_resourceManager(_config.get("Engine", "resource_path")) {

	_window.set_callback([this](platform::Window::Event event) {
		switch (event) {
		case platform::Window::Event::CLOSE:
			_state = State::TERMINATED;
			break;
		case platform::Window::Event::LOSTFOCUS:
			if (_state != State::TERMINATED)
				_state = State::PAUSED;
			break;
		case platform::Window::Event::GAINFOCUS:
			_state = State::RUNNING;
			break;
		}
	});
}

redox::Application::~Application() {
}

void redox::Application::run() {
	_window.show();
	_state = State::RUNNING;

	const u32 max_fps = _config.get("Engine", "max_fps");
	const auto timestep = 1000. / max_fps;

	_timer.start();

	while (_state != State::TERMINATED) {
		auto dt_ms = _timer.elapsed();

		_window.process_events();
		_inputSystem.poll();

		if (_state == State::PAUSED) {
			RDX_SLEEP_MS(1);
			continue;
		}

		if (dt_ms >= timestep) {
			_timer.reset();
			_renderSystem.render();

			_window.set_title(
				redox::format("redox engine | {0}fps", 1000. / dt_ms));
		}
	}
}

void redox::Application::stop() {
	_state = State::TERMINATED;
}

const redox::Configuration& redox::Application::config() const {
	return _config;
}

const redox::platform::Timer& redox::Application::timer() const {
	return _timer;
}

const redox::ResourceManager& redox::Application::resource_manager() const {
	return _resourceManager;
}
