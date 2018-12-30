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

//#define RDX_INPUT_DEBUG_KEY_CODES

struct redox::input::InputSystem::internal {
	Hashmap<Keys, KeyState> keyStates;
	bool useHighDpi;

	void handle_wm_input(const MSG& msg);
	void handle_wm_key_du(const MSG& msg);
};

redox::input::InputSystem::InputSystem(const platform::Window& window) :
	_internal(make_unique<internal>()) {
	_internal->useHighDpi = Application::instance->config()->get("Input", "HighDpi");

	RDX_LOG("Initializing Input System...", ConsoleColor::GREEN);

	if (_internal->useHighDpi) {
		RDX_LOG("Registering High DPI devices...");
		RAWINPUTDEVICE keyboardDevice;
		keyboardDevice.usUsagePage = 0x01;
		keyboardDevice.usUsage = 0x06;
		keyboardDevice.dwFlags = RIDEV_NOLEGACY;
		keyboardDevice.hwndTarget = reinterpret_cast<HWND>(window.native_handle());

		const RAWINPUTDEVICE devices[] = { keyboardDevice };

		if (!RegisterRawInputDevices(devices, util::array_size<UINT>(devices), sizeof(RAWINPUTDEVICE)))
			throw Exception("failed to register raw device");
	}
}

redox::input::InputSystem::~InputSystem() {
}

void redox::input::InputSystem::internal::handle_wm_input(const MSG& msg) {
	HRAWINPUT handle = reinterpret_cast<HRAWINPUT>(msg.lParam);

	UINT dwSize;
	GetRawInputData(handle, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	Buffer<u8> buffer(dwSize);
	GetRawInputData(handle, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());
	if (raw->header.dwType == RIM_TYPEKEYBOARD) {
		auto& kbData = raw->data.keyboard;
		auto mappingIt = g_vkey_mappings.find(kbData.VKey);
		if (mappingIt == g_vkey_mappings.end()) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
			RDX_DEBUG_LOG("Unknown VKey: {0}", kbData.VKey);
#endif
			return;
		}

		if (kbData.Flags == RI_KEY_MAKE) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
			RDX_DEBUG_LOG("VKey Pressed: {0}", kbData.VKey);
#endif
			keyStates.insert({ mappingIt->second, KeyState::PRESSED });
		}
		else if (kbData.Flags == RI_KEY_BREAK) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
			RDX_DEBUG_LOG("VKey Released: {0}", kbData.VKey);
#endif
			keyStates.insert({ mappingIt->second, KeyState::RELEASED });
		}
	}
}

void redox::input::InputSystem::internal::handle_wm_key_du(const MSG& msg) {
	auto mappingIt = g_vkey_mappings.find(static_cast<u32>(msg.wParam));
	if (mappingIt == g_vkey_mappings.end()) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
		RDX_DEBUG_LOG("Unknown VKey: {0}", msg.wParam);
#endif
		return;
	}

	if (msg.message == WM_KEYDOWN) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
		RDX_DEBUG_LOG("VKey Pressed: {0}", msg.wParam);
#endif
		keyStates.insert({ mappingIt->second, KeyState::PRESSED });
	}
	else if (msg.message == WM_KEYUP) {
#ifdef RDX_INPUT_DEBUG_KEY_CODES
		RDX_DEBUG_LOG("VKey Released: {0}", msg.wParam);
#endif
		keyStates.insert({ mappingIt->second, KeyState::RELEASED });
	}
}

void redox::input::InputSystem::poll() {
	_internal->keyStates.clear();

	MSG msg;
	while (PeekMessage(&msg, NULL, WM_INPUT, WM_KEYLAST, PM_REMOVE | PM_QS_INPUT)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (_internal->useHighDpi) {
			if (msg.message == WM_INPUT) {
				_internal->handle_wm_input(msg);
			}
		}
		else {
			if (msg.message == WM_KEYDOWN || msg.message == WM_KEYUP) {
				_internal->handle_wm_key_du(msg);
			}
		}
	}
}

redox::input::KeyState redox::input::InputSystem::key_state(Keys key) const {
	if (auto state = _internal->keyStates.find(key); state != _internal->keyStates.end())
		return state->second;

	return KeyState::NORMAL;
}
#endif