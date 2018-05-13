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
#include <type_traits>

namespace redox {
	namespace util {
		template<class C, class T, std::size_t Size >
		constexpr auto array_size(const T(&array)[Size]) noexcept {
			return static_cast<C>(Size);
		}

		//We want to "reload" (i.e. destructing and constructing again) a RAII 
		//object without introducing "two-phase" (de-)initialization. It's 
		//probably not the cleanest way, but it's a good solution for now.

		template<class T, class...Args>
		constexpr void reconstruct(T& ref, Args&&...args) {
			ref.~T();
			new (&ref) T(std::forward<Args>(args)...);
		}

		template<class M, class T>
		constexpr auto offset_of(M T::* p, T sample = T()) {
			return reinterpret_cast<std::size_t>(&(sample.*p))
				- reinterpret_cast<std::size_t>(&sample);
		}

		template<std::size_t Index, class T>
		constexpr auto check_bit(T expr) noexcept {
			return static_cast<bool>((expr >> Index) & 0x1);
		}
	}

	//Define bit flag operations for any POW2-indexed strongly-typed enum
	//the global bit ops need to be present in ::redox namespace

#define RDX_ENABLE_ENUM_FLAGS(en) 						\
	template<>											\
	struct enable_bit_flags<en> : std::true_type {};	\

	//---

	template<class Enum>
	struct enable_bit_flags : std::false_type {
	};

	template<typename Enum>
	constexpr std::enable_if_t<enable_bit_flags<Enum>::value, Enum> operator| (Enum a, Enum b) {
		using ut = typename std::underlying_type_t<Enum>;
		return static_cast<Enum>(static_cast<ut>(a) | static_cast<ut>(b));
	}

	template<typename Enum>
	constexpr std::enable_if_t<enable_bit_flags<Enum>::value, Enum> operator& (Enum a, Enum b) {
		using ut = typename std::underlying_type_t<Enum>;
		return static_cast<Enum>(static_cast<ut>(a) & static_cast<ut>(b));
	}
}