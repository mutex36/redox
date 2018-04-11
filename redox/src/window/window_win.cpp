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
#include "window.h"

#define RDX_LOG_TAG "WindowSystem"
#include "core\logging\log.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "core\sys\windows.h"

struct redox::Window::PIMPL {
	HWND handle;
	HINSTANCE instance;
	String classname;
};

LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	if (msg == WM_CREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	LONG_PTR wndptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	auto instance = reinterpret_cast<redox::Window*>(wndptr);

	if (instance != nullptr) {
		switch (msg) {
			case WM_CLOSE: {
				instance->_notify_event(redox::Window::Event::CLOSE);
				break;
			}
			case WM_SYSCOMMAND: {
				auto cmd = wp & 0xfff0;
				if (cmd == SC_MINIMIZE)
					instance->_notify_event(redox::Window::Event::MINIMIZE);
				break;
			}
		}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

redox::Window::Window(const String& title, const Size& size) {
	_pimpl = make_smart_ptr<PIMPL>();
	_pimpl->instance = GetModuleHandle(0);
	_pimpl->classname = "redox_window";

	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	WNDCLASS wndClass{};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = _pimpl->instance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_HAND);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszClassName = _pimpl->classname.cstr();

	RegisterClass(&wndClass);

	RECT r{ 0, 0, size.width, size.height };
	AdjustWindowRect(&r, dwStyle, FALSE);

	_pimpl->handle = CreateWindowEx(
		NULL, _pimpl->classname.cstr(), title.cstr(), dwStyle,
		r.left, r.left, r.right - r.left, r.bottom - r.top,
		NULL, NULL, _pimpl->instance, this);
}


redox::Window::~Window() {
	DestroyWindow(_pimpl->handle);
	UnregisterClass(_pimpl->classname.cstr(), _pimpl->instance);
}

void redox::Window::show() {
	//SetFocus(_pimpl->handle);
	//SetForegroundWindow(_pimpl->handle);
	ShowWindow(_pimpl->handle, SW_SHOW);
}

void redox::Window::process_events() {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void redox::Window::hide() {
	ShowWindow(_pimpl->handle, SW_HIDE);
}

void redox::Window::set_title(const String& title) {
	SetWindowText(_pimpl->handle, title.cstr());
}

void redox::Window::event_callback(EventFn&& fn) {
	_eventfn = fn;
}

void redox::Window::_notify_event(const Event ev) {
	if (_eventfn) 
		_eventfn(ev);
}
#endif