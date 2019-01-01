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
#include <core\core.h>
#include <core\string_format.h>

#define RDX_LOG(fmt, ...) redox::detail::log(fmt,  __VA_ARGS__)

#ifdef RDX_DEBUG
#define RDX_DEBUG_LOG(fmt, ...) redox::detail::debug_log(##fmt##"\n",  __VA_ARGS__)
#else
#define RDX_DEBUG_LOG(...)
#endif

#define RDX_ASSERT(a) RDX_ASSERT_TRUE(a)

#define RDX_ASSERT_EQ(a,b)				\
if (!redox::detail::assert_eq(a,b))		\
	RDX_DEBUG_BREAK();					\

#define RDX_ASSERT_NEQ(a,b)				\
if (!redox::detail::assert_neq(a,b))	\
	RDX_DEBUG_BREAK();					\

#define RDX_ASSERT_TRUE(a)				\
if (!redox::detail::assert_true(a))		\
	RDX_DEBUG_BREAK();					\

#define RDX_ASSERT_FALSE(a)				\
if (!redox::detail::assert_false(a))	\
	RDX_DEBUG_BREAK();					\

#define RDX_ASSERT_MAIN_THREAD															\
RDX_ASSERT(::redox::Application::instance->main_thread() == std::this_thread::get_id()); \

namespace redox {

	enum class ConsoleColor {
		RED, GREEN, BLUE, WHITE, GRAY
	};

	namespace detail {
		void impl_debug_log(const String& string);
		void impl_set_console_color(redox::ConsoleColor color);
		void impl_restore_console_color();

		template<class...Args>
		RDX_INLINE void debug_log(redox::StringView fmts, const Args&...args) {
			auto fmt = redox::format(fmts, args...);
			impl_debug_log(fmt);
		}

		template<class...Args>
		RDX_INLINE void log(redox::StringView fmts, const Args&...args) {
			auto fmt = redox::format(fmts, args...);
			auto fst = redox::format("[{0}] {1}", std::chrono::system_clock::now(), fmt);
			std::puts(fst.c_str());
		}

		template<class...Args>
		RDX_INLINE void log(redox::StringView fmts, redox::ConsoleColor color, const Args&...args) {
			impl_set_console_color(color);
			log(fmts, args...);
			impl_restore_console_color();
		}

		template<class T1>
		RDX_INLINE bool assert_true(const T1& a) {
			if (a) return true;
			log("Assertion failed: {0} == true\n", ConsoleColor::RED, a);
			return false;
		}

		template<class T1>
		RDX_INLINE bool assert_false(const T1& a) {
			if (!a) return true;
			log("Assertion failed: {0} == false\n", ConsoleColor::RED, a);
			return false;
		}

		template<class T1, class T2>
		RDX_INLINE bool assert_eq(const T1& a, const T2& b) {
			if (a == b) return true;
			log("Assertion failed: {0} == {1}\n", ConsoleColor::RED, a, b);
			return false;
		}

		template<class T1, class T2>
		RDX_INLINE bool assert_neq(const T1& a, const T2& b) {
			if (a != b) return true;
			log("Assertion failed: {0} != {1}\n", ConsoleColor::RED, a, b);
			return false;
		}
	}
}