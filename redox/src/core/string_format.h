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

#include <stdlib.h> //std::strtof, std::strtoll
#include <cstdlib> //std::itoa

#include <charconv>

namespace redox {

	template<class T>
	_RDX_INLINE T parse(StringView expr) {
		return expr;
	}

	template<>
	_RDX_INLINE String parse(StringView expr) {
		return expr.data();
	}

	template<>
	_RDX_INLINE f32 parse(StringView expr) {
		return std::strtof(expr.data(), NULL);
	}

	template<>
	_RDX_INLINE i64 parse(StringView expr) {
		return std::strtoll(expr.data(), NULL, 10);
	}

	template<>
	_RDX_INLINE i32 parse(StringView expr) {
		return std::strtol(expr.data(), NULL, 10);
	}

	template<>
	_RDX_INLINE long parse(StringView expr) {
		return std::strtol(expr.data(), NULL, 10);
	}

	template<>
	_RDX_INLINE u64 parse(StringView expr) {
		return std::strtoull(expr.data(), NULL, 10);
	}

	template<>
	_RDX_INLINE u32 parse(StringView expr) {
		return std::strtoul(expr.data(), NULL, 10);
	}

	template<>
	_RDX_INLINE bool parse(StringView expr) {
		if (!expr.empty()) {
			if (expr[0] == '1' || expr == "true")
				return true;

			if (expr[0] == '0' || expr == "false")
				return false;
		}

		throw Exception("parsing error");
	}

	template<class T>
	struct binary { const T& value; };

	template<class T>
	_RDX_INLINE redox::String lexical_cast(const binary<T>& expr) {
		redox::String output;
		output.reserve(sizeof(T) * 8);
		for (std::size_t i = 0; i < output.capacity(); ++i)
			output += ((expr.value >> i) & 0x1) ? "1" : "0";
		return output;
	}

	_RDX_INLINE redox::String lexical_cast(const f64& expr) {
		i8 result[16]; //TODO: unsafe
		sprintf_s(result, "%.3f", expr);
		return result;
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

	_RDX_INLINE redox::String lexical_cast(const char* expr) {
		return expr;
	}

	_RDX_INLINE redox::String lexical_cast(StringView expr) {
		return expr.data();
	}

	template<class...Args>
	String format(StringView format, const Args&...args) {
		return format.data();
	}

	template<class Arg0, class...Args>
	String format(StringView format, const Arg0& arg0, const Args&...args) {
		Buffer<String> parsed{ { lexical_cast(arg0), lexical_cast(args)... } };
		std::size_t s0 = -1; std::size_t s1 = 0;

		//Roughly approximate the size of the result string
		//This prevents expensive reallocations during formatting
		String output;
		output.reserve(format.size() + (sizeof...(Args) + 1) * 3);

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