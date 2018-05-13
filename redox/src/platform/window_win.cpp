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
#include "window.h"
#include "core\logging\log.h"
#include "platform\windows.h"
#include "resources\helper.h"

#define RDX_LOG_TAG "WindowSystem"

struct redox::platform::Window::internal {
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
	auto instance = reinterpret_cast<redox::platform::Window*>(wndptr);

	if (instance != nullptr) {
		switch (msg) {
			case WM_CLOSE: {
				instance->_notify_event(redox::platform::Window::Event::CLOSE);
				break;
			}
			case WM_SYSCOMMAND: {
				if ((wp & 0xfff0) == SC_MINIMIZE)
					instance->_notify_event(redox::platform::Window::Event::MINIMIZE);
				break;
			}
			case WM_KILLFOCUS: {
				instance->_notify_event(redox::platform::Window::Event::LOSTFOCUS);
				break;
			}
			case WM_SETFOCUS: {
				instance->_notify_event(redox::platform::Window::Event::GAINFOCUS);
				break;
			}
		}
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

redox::platform::Window::Window(const String& title,
	const Bounds& bounds, const Appearance& appearance) : _bounds(bounds) {
	_internal = make_smart_ptr<internal>();
	_internal->instance = GetModuleHandle(0);
	_internal->classname = "redox_window";

	DWORD dwStyle = WS_SYSMENU | WS_MINIMIZEBOX;

	auto iconFile = ResourceHelper::instance().resolve_path(appearance.icon);
	auto icon = (HICON)LoadImage(NULL, iconFile.cstr(), IMAGE_ICON,
		0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

	WNDCLASS wndClass{};
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = _internal->instance;
	wndClass.hIcon = icon;
	wndClass.hCursor = LoadCursor(NULL, IDC_HAND);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszClassName = _internal->classname.cstr();

	RegisterClass(&wndClass);

	RECT windowRect{ 0,0 };
	if (appearance.fullscreen) {
		windowRect.right = GetSystemMetrics(SM_CXSCREEN);
		windowRect.bottom = GetSystemMetrics(SM_CYSCREEN);
		dwStyle |= WS_POPUP;
	} else {
		windowRect.right = _bounds.width;
		windowRect.bottom = _bounds.height;
		AdjustWindowRect(&windowRect, dwStyle, FALSE);
		dwStyle |= WS_OVERLAPPEDWINDOW | WS_CAPTION;
	}

	_internal->handle = CreateWindowEx(
		NULL, _internal->classname.cstr(), title.cstr(), dwStyle,
		windowRect.left, windowRect.left, 
		windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, _internal->instance, this);
}

redox::platform::Window::~Window() {
	DestroyWindow(_internal->handle);
	UnregisterClass(_internal->classname.cstr(), _internal->instance);
}

void redox::platform::Window::show() const {
	ShowWindow(_internal->handle, SW_SHOW);
	SetFocus(_internal->handle);
	SetForegroundWindow(_internal->handle);
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
	_process_events(_internal->handle, 0, WM_INPUT);
	_process_events(_internal->handle, WM_KEYLAST, 0);
}

void redox::platform::Window::hide() const {
	ShowWindow(_internal->handle, SW_HIDE);
}

void redox::platform::Window::set_title(const String& title) {
	SetWindowText(_internal->handle, title.cstr());
}

void redox::platform::Window::set_callback(EventFn && fn) {
	_eventfn = std::move(fn);
}

bool redox::platform::Window::is_minimized() const {
	return IsIconic(_internal->handle);
}

redox::platform::Window::Bounds redox::platform::Window::bounds() const {
	return _bounds;
}

void* redox::platform::Window::native_handle() const {
	return _internal->handle;
}

void redox::platform::Window::_notify_event(const Event ev) {
	if (_eventfn)
		_eventfn(ev);
}

#endif