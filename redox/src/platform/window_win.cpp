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
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include <platform/window.h>
#include <core/logging/log.h>
#include <core/application.h>
#include <platform/windows.h>
#include <resources/resource.h>

struct redox::platform::Window::internal {
	HWND handle;
	HINSTANCE instance;
	String classname;
	EventFn eventFn;
	bool closed{ false };

	HCURSOR get_cursor(const String& cursorName);
	void notify_event(const Event ev);

	static LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};

LRESULT redox::platform::Window::internal::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	if (msg == WM_CREATE) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		return DefWindowProc(hwnd, msg, wp, lp);
	}

	LONG_PTR wndptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	auto instance = reinterpret_cast<redox::platform::Window*>(wndptr);
	if (instance != nullptr) {
		auto& internal = *instance->_internal;
		switch (msg) {
			case WM_CLOSE: {
				internal.notify_event(redox::platform::Window::Event::CLOSE);
				internal.closed = true;
				break;
			}
			case WM_SYSCOMMAND: {
				if ((wp & 0xfff0) == SC_MINIMIZE)
					internal.notify_event(redox::platform::Window::Event::MINIMIZE);
				break;
			}
			case WM_KILLFOCUS: {
				internal.notify_event(redox::platform::Window::Event::LOSTFOCUS);
				break;
			}
			case WM_SETFOCUS: {
				internal.notify_event(redox::platform::Window::Event::GAINFOCUS);
				break;
			}
		}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

redox::platform::Window::Window(const WindowSettings& settings) :
	_internal(std::make_unique<internal>()) {

	_internal->instance = GetModuleHandle(0);
	_internal->classname = "RedoxWindow";

	DWORD dwStyle = WS_SYSMENU | WS_MINIMIZEBOX;

	auto resources = Application::instance->resource_manager();
	Path iconFile = resources->resolve_path(settings.iconPath);

	auto icon = (HICON)LoadImageW(NULL, iconFile.c_str(), IMAGE_ICON,
		0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

	WNDCLASS wndClass{};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = internal::WndProc;
	wndClass.hInstance = _internal->instance;
	wndClass.hIcon = icon;
	wndClass.hCursor = _internal->get_cursor(settings.defaultCursor);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszClassName = _internal->classname.c_str();

	RegisterClass(&wndClass);
	RECT windowRect{ 0,0 };
	if (util::check_flag(settings.flags, WindowFlags::FULLSCREEN)) {
		windowRect.right = GetSystemMetrics(SM_CXSCREEN);
		windowRect.bottom = GetSystemMetrics(SM_CYSCREEN);
		dwStyle |= WS_POPUP;
	} else {
		windowRect.right = settings.width;
		windowRect.bottom = settings.height;
		AdjustWindowRect(&windowRect, dwStyle, FALSE);

		if (util::check_flag(settings.flags, WindowFlags::RESIZABLE)) {
			dwStyle |= WS_OVERLAPPEDWINDOW;
		} else {
			dwStyle |= WS_OVERLAPPED;
		}
	}

	DWORD dwExStyle{ 0 };
	if (settings.stayOnTop) {
		dwExStyle |= WS_EX_TOPMOST;
	}

	_internal->handle = CreateWindowEx(
		dwExStyle, _internal->classname.c_str(), settings.title.c_str(), dwStyle,
		windowRect.left, windowRect.left, 
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, _internal->instance, this);
}

redox::platform::Window::~Window() {
	DestroyWindow(_internal->handle);
	UnregisterClass(_internal->classname.c_str(), _internal->instance);
}

void redox::platform::Window::show() const {
	ShowWindow(_internal->handle, SW_SHOW);
	SetFocus(_internal->handle);
	SetForegroundWindow(_internal->handle);
}

bool redox::platform::Window::is_closed() {
	return _internal->closed;
}

void _process_events(HWND hwnd, UINT rangeMin, UINT rangeMax) {
	MSG msg;
	while (PeekMessage(&msg, hwnd, rangeMin, rangeMax, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void redox::platform::Window::process_events() const {
	//We want to skip all keyboard/mouse/input related
	//messages. These are processed and handled by the input manager
	_process_events(_internal->handle, 0, WM_INPUT - 1);
	_process_events(_internal->handle, WM_KEYLAST, 0);
}

void redox::platform::Window::hide() const {
	ShowWindow(_internal->handle, SW_HIDE);
}

void redox::platform::Window::set_title(const String& title) {
	SetWindowText(_internal->handle, title.c_str());
}

void redox::platform::Window::set_callback(EventFn && fn) {
	_internal->eventFn = std::move(fn);
}

bool redox::platform::Window::is_minimized() const {
	return IsIconic(_internal->handle);
}

void* redox::platform::Window::native_handle() const {
	return _internal->handle;
}

void redox::platform::Window::internal::notify_event(const Event ev) {
	if (eventFn)
		eventFn(ev);
}

HCURSOR redox::platform::Window::internal::get_cursor(const String& cursorName) {
	if (cursorName == "Hand")
		return LoadCursor(NULL, IDC_HAND);

	if (cursorName == "Arrow")
		return LoadCursor(NULL, IDC_ARROW);

	if (cursorName == "Cross")
		return LoadCursor(NULL, IDC_CROSS);

	//TODO: implement more
	return LoadCursor(NULL, IDC_HAND);
}


#endif