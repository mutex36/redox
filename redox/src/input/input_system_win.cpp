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
#include "mappings\mappings_win.h"
#include "input_system.h"

#include "platform\windows.h"
#include "core\application.h"

#define RDX_LOG_TAG "InputSystem"

redox::input::InputSystem::InputSystem(const platform::Window& window) : _windowRef(window) {
	RDX_LOG("Registering input devices...");

#ifdef RDX_INPUT_HIGH_DPI
	RAWINPUTDEVICE keyboardDevice;
	keyboardDevice.usUsagePage = 1;
	keyboardDevice.usUsage = 6;
	keyboardDevice.dwFlags = 0;
	keyboardDevice.hwndTarget = reinterpret_cast<HWND>(window.native_handle());

	const RAWINPUTDEVICE devices[] = { keyboardDevice };

	if (!RegisterRawInputDevices(devices, util::array_size<UINT>(devices), sizeof(RAWINPUTDEVICE)))
		throw Exception("failed to register raw device");
#endif
}

redox::input::InputSystem::~InputSystem() {
}

void redox::input::InputSystem::poll() {
	_keyStates.clear();

	MSG msg;
	while (PeekMessage(&msg, reinterpret_cast<HWND>(_windowRef.native_handle()), 
		WM_INPUT, WM_KEYLAST, PM_REMOVE | PM_QS_INPUT)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

#ifdef RDX_INPUT_HIGH_DPI
		if (msg.message == WM_INPUT) {
			HRAWINPUT handle = reinterpret_cast<HRAWINPUT>(msg.lParam);

			UINT dwSize;
			GetRawInputData(handle, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			Buffer<u8> buffer(dwSize);
			GetRawInputData(handle, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());
			switch (raw->header.dwType) {
			case RIM_TYPEKEYBOARD:
			{
				auto& kbData = raw->data.keyboard;
				auto mapping = g_vkey_mappings.get(kbData.VKey);

				if (!mapping) {
					RDX_LOG("Unknown VKey code: {0}", kbData.VKey);
					continue;
				}

				switch (kbData.Flags) {
				case RI_KEY_MAKE:
					_keyStates.push(mapping.value(), KeyState::PRESSED);
					break;
				case RI_KEY_BREAK: {
					_keyStates.push(mapping.value(), KeyState::RELEASED);
					break;
				}}
				break;
			}}
		}
	}
#else

	if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {

		auto mapping = g_vkey_mappings.get(msg.wParam);
		if (!mapping) {
			RDX_LOG("Unknown VKey code: {0}", msg.wParam);
			continue;
		}
			
		if (msg.message == WM_KEYDOWN)
			_keyStates.push(mapping.value(), KeyState::PRESSED);
		else if (msg.message == WM_KEYUP)
			_keyStates.push(mapping.value(), KeyState::RELEASED);
	}

#endif
}

redox::input::KeyState redox::input::InputSystem::key_state(Keys key) {
	auto state = _keyStates.get(key);
	return state.value_or(KeyState::NORMAL);
}
#endif