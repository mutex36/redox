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
#include "core\string.h"
#include "core\string_format.h"
#include "platform\windows.h"

namespace redox::detail {
	static const HANDLE std_handle = GetStdHandle(STD_OUTPUT_HANDLE);

	template<class...Args>
	_RDX_INLINE void log(const redox::StringView& fmts, const Args&...args) {
		auto fmt = format(fmts, args...);
		WriteConsole(detail::std_handle, fmt.cstr(), 
			static_cast<DWORD>(fmt.size()), NULL, NULL);
	}

	template<class T1>
	_RDX_INLINE bool assert_true(const T1& a) {
		if (a) return true;
		log("Assertion failed: {0} == true\n", a);
		return false;
	}

	template<class T1>
	_RDX_INLINE bool assert_false(const T1& a) {
		if (!a) return true;
		log("Assertion failed: {0} == false\n", a);
		return false;
	}

	template<class T1, class T2>
	_RDX_INLINE bool assert_eq(const T1& a, const T2& b) {
		if (a == b) return true;
		log("Assertion failed: {0} == {1}\n", a, b);
		return false;
	}

	template<class T1, class T2>
	_RDX_INLINE bool assert_neq(const T1& a, const T2& b) {
		if (a != b) return true;
		log("Assertion failed: {0} != {1}\n", a, b);
		return false;
	}

	template<class...Args>
	_RDX_INLINE void debug_log(const redox::StringView& fmts, const Args&...args) {
		auto fmt = format(fmts, args...);
		OutputDebugString(fmt.cstr());
	}
}
#endif