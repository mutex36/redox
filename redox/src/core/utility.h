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

#define _RDX_HELPER_CONCAT_IMPL(x,y) x##y
#define _RDX_HELPER_CONCAT(x,y) _RDX_HELPER_CONCAT_IMPL(x,y)

#define RDX_ENABLE_ENUM_FLAGS(en) 								\
	template<>													\
	struct ::redox::enable_bit_flags<en> : std::true_type {};	\


#define RDX_SCOPE_GUARD(fn)															\
auto _RDX_HELPER_CONCAT(_scope_guard_, __COUNTER__) = redox::make_scope_guard(fn)	\

namespace redox {
	template<class Enum>
	struct enable_bit_flags : std::false_type {};

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

	template<class Fn>
	struct scope_guard {
		scope_guard(Fn&& fn) : _fn(std::move(fn)) {}
		~scope_guard() { _fn(); }

		Fn _fn;
	};

	template<class Fn>
	scope_guard<Fn> make_scope_guard(Fn&& fn) {
		return scope_guard<Fn>(std::forward<Fn>(fn));
	}

	template<class M>
	struct member_type_wrapper {
		using type = M;
	};

	template <class T, class M>
	member_type_wrapper<M> member_type(M T:: *);

	struct static_instance_wrapper {
		template<class T>
		static_instance_wrapper(T* this_ptr) {
			T::instance = this_ptr;
		}
	};

	template<class Container>
	auto byte_size(const Container& cnt) {
		return cnt.size() * sizeof(Container::value_type);
	}
}

//####UTIL####

namespace redox::util {
	template<class C, class T, std::size_t Size >
	constexpr auto array_size(const T(&array)[Size]) noexcept {
		return static_cast<C>(Size);
	}

	template<class C, class M, class T>
	constexpr auto offset_of(M T::* p, T sample = T()) {
		return static_cast<C>(reinterpret_cast<std::size_t>(&(sample.*p))
			- reinterpret_cast<std::size_t>(&sample));
	}

	template<std::size_t Index, class T>
	constexpr auto check_bit(T expr) noexcept {
		return static_cast<bool>((expr >> Index) & 0x1);
	}
}