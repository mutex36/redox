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

redox::Application::Application() 
	: _window("redox engine", { 500 , 500 }), _renderer(_window) {


	//TODO: demo
	_renderer.demo_setup();

	_window.set_callback([this](Window::Event event) {
		switch (event) {
		case redox::Window::Event::CLOSE:
			_running = false;
			break;
		}
	});
}

redox::Application::~Application() {
}

void redox::Application::run() {
	_window.show();
	_timer.start();
	_running = true;

	while (_running) {
		_timer.reset();
		_window.process_events();

		if (_window.is_minimized())
			continue;

		_renderer.render();

		auto delta = _timer.elapsed();
		auto fps = 1000. / delta;
		_window.set_title(
			redox::format("redox engine | {0}fps", fps));
	}

	_renderer.wait_pending();
}
