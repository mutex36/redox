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
#pragma once
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "log.h"
#include <platform\windows.h>

namespace redox::detail {
	static const HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	static CONSOLE_SCREEN_BUFFER_INFO restore;

	//https://i.imgur.com/oRvcm5L.png
	static const Hashmap<ConsoleColor, WORD> color_mappings = {
		{ConsoleColor::RED, 4},
		{ConsoleColor::GREEN, 11},
		{ConsoleColor::BLUE, 9},
		{ConsoleColor::WHITE, 15},
		{ConsoleColor::GRAY, 8},
	};

	void impl_set_console_color(redox::ConsoleColor color) {
		auto it = color_mappings.find(color);
		if (it != color_mappings.end()) {
			GetConsoleScreenBufferInfo(std_handle, &restore);
			SetConsoleTextAttribute(std_handle, it->second);
		}
	}

	void impl_restore_console_color() {
		SetConsoleTextAttribute(std_handle, restore.wAttributes);
	}

	void impl_debug_log(const redox::String& str) {
		OutputDebugString(str.c_str());
	}
}
#endif