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
#include "core\string.h"
#include "core\buffer.h"

#include <stdlib.h> //std::strtof, std::strtoll
#include <cstdlib> //std::itoa

namespace redox {

	template<class T>
	_RDX_INLINE T parse(const String&) {
		static_assert(0, "unsupported type"); 
	}

	template<>
	_RDX_INLINE f32 parse(const String& expr) {
		return std::strtof(expr.cstr(), NULL);
	}

	template<>
	_RDX_INLINE i64 parse(const String& expr) {
		return std::strtoll(expr.cstr(), NULL, 10);
	}

	template<>
	_RDX_INLINE u64 parse(const String& expr) {
		return std::strtoull(expr.cstr(), NULL, 10);
	}

	namespace detail {
		template<class T>
		struct binary { const T& value; };
	}

	template<class T>
	_RDX_INLINE detail::binary<T> binary(const T& value) {
		return detail::binary<T>{value};
	}

	template<class T>
	_RDX_INLINE redox::String lexical_cast(const detail::binary<T>& expr) {
		redox::String output(sizeof(T) * 8);
		for (std::size_t i = 0; i < output.capacity(); ++i)
			output += ((expr.value >> i) & 0x1) ? "1" : "0";
		return output;
	}

	_RDX_INLINE redox::String lexical_cast(const f64& expr) {
		i8 buffer[_CVTBUFSIZE];
		_gcvt_s(buffer, expr, 15);
		return buffer;
	}

	_RDX_INLINE redox::String lexical_cast(const i32& expr) {
		i8 buffer[16]; //TODO: unsafe
		_itoa_s(expr, buffer, 10);
		return buffer;
	}

	_RDX_INLINE redox::String lexical_cast(const i64& expr) {
		 i8 buffer[16]; //TODO: unsafe
		_i64toa_s(expr, buffer, sizeof(buffer), 10);
		return buffer;
	}

	_RDX_INLINE redox::String lexical_cast(const u64& expr) {
		i8 buffer[16]; //TODO: unsafe
		_ui64toa_s(expr, buffer, sizeof(buffer), 10);
		return buffer;
	}

	_RDX_INLINE redox::String lexical_cast(const u32& expr) {
		i8 buffer[16]; //TODO: unsafe
		_itoa_s(expr, buffer, sizeof(buffer), 10);
		return buffer;
	}

	template<std::size_t N>
	_RDX_INLINE redox::String lexical_cast(const char(&expr)[N]) {
		return static_cast<const char*>(expr);
	}

	_RDX_INLINE redox::String lexical_cast(const bool& expr) {
		return expr ? "true" : "false";
	}

	_RDX_INLINE redox::String lexical_cast(const String& expr) {
		return expr;
	}

	_RDX_INLINE String format(const String& format) {
		return format;
	}

	template<class...Args>
	String format(const String& format, const Args&...args) {
		Buffer<String> parsed = { lexical_cast(args)... };
		std::size_t s0 = -1; std::size_t s1 = 0;
		String output;

		for (std::size_t i = 0; i < format.size(); ++i) {
			if (format[i] == '{') {
				output += format.substr(s1, i - s1);
				s0 = i; continue;
			}
			if (format[i] == '}') {
				if (s0 == -1) throw Exception("unexpected token"); 

				auto idstr = format.substr(s0 + 1, i - s0 - 1);
				auto argindex = parse<std::size_t>(idstr);

				output += parsed[argindex];
				s0 = -1; s1 = i + 1;
			}
		}

		output += format.substr(s1);
		return output;
	}
}