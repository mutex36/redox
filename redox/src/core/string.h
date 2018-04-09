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
#include "core.h"

#include <type_traits> //std::aligned_storage
#include <stdexcept> //std::bad_alloc
#include <cstring> //std::strlen

#include "allocator\default_alloc.h"
#include "core\non_copyable.h"

namespace redox {
	namespace detail {

		template<class CT, class Allocator>
		class String {
		public:
			using value_type_t = CT;

			_RDX_INLINE String() : _size(0), _reserved(0), _data(nullptr) {
			}

			String(const value_type_t* str, const std::size_t length) : String() {
				reserve(length);
				_size = length;
				for (size_t i = 0; i < _size; i++)
					at(i) = str[i];

				if (_size > 0)
					_zero_terminate();
			}

			_RDX_INLINE explicit String(const std::size_t rsv) : String() {
				reserve(rsv);
			}

			_RDX_INLINE String(const value_type_t* str) : String(str, std::strlen(str)) {
			}

			_RDX_INLINE String(String&& ref) : _data(ref._data), _reserved(ref._reserved), _size(ref._size) {
				ref._data = nullptr;

#ifdef RDX_DEBUG
				//Technically, moved-from objects are left in an undefined
				//state, so this is not exactly needed. But it improves debugging.
				ref._size = 0;
				ref._reserved = 0;
#endif
			}

			_RDX_INLINE String& operator=(String&& ref) {
				_data = ref._data;
				_reserved = ref._reserved;
				_size = ref._size;

				ref._data = nullptr;

#ifdef RDX_DEBUG
				//Technically, moved-from objects are left in an undefined
				//state, so this is not exactly needed. But it improves debugging.
				ref._size = 0;
				ref._reserved = 0;
#endif
				return *this;
			}

			_RDX_INLINE String(const String& ref) : String(ref._data, ref._size) {
			}

			_RDX_INLINE String(const String& lhs, const String& rhs) {
				//TODO
			}

			_RDX_INLINE String& operator=(const String& ref) {
				//TODO
				return *this;
			}

			_RDX_INLINE String operator+(const String& rhs) {
				return { *this, rhs };
			}

			_RDX_INLINE ~String() {
				_dealloc();
			}

			_RDX_INLINE void reserve(const std::size_t size) {
				if (size > _reserved) {
					_dealloc();
					_data = Allocator::allocate(size + 1);
					_reserved = size;
				}
			}

			_RDX_INLINE String substr(const std::size_t off, const std::size_t size) const {
				return { _data + off, size };
			}

			_RDX_INLINE value_type_t& operator[](const std::size_t index) {
				return at(index);
			}

			_RDX_INLINE value_type_t& at(const std::size_t index) {
				return _data[index];
			}

			_RDX_INLINE const value_type_t& operator[](const std::size_t index) const {
				return at(index);
			}

			_RDX_INLINE const value_type_t& at(const std::size_t index) const {
				return _data[index];
			}

			_RDX_INLINE std::size_t size() const {
				return _size;
			}

			_RDX_INLINE const value_type_t* cstr() const {
				if (_data == nullptr)
					return _empty;
				return _data;
			}

			//_RDX_INLINE value_type_t* begin() const {
			//	return _data;
			//}

			//_RDX_INLINE value_type_t* end() const {
			//	return _data + _size;
			//}

		private:
			_RDX_INLINE void _dealloc() {
				Allocator::deallocate(_data);
			}

			_RDX_INLINE void _zero_terminate() {
				at(_size) = '\0';
			}

			value_type_t* _data;
			const value_type_t _empty[1] = { '\0' };

			std::size_t _size;
			std::size_t _reserved;
		};
	}

	using String = detail::String<i8, DefaultAllocator<i8>>;
}