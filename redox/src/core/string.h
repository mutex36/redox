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
#include "allocation\default_allocator.h"

#include <type_traits> //std::aligned_storage
#include <cstring> //std::strlen, std::memcpy

namespace redox {
	namespace detail {

		template<class CT,
			class Allocator = allocation::DefaultAllocator<CT>>
		class String {
		public:
			_RDX_INLINE String() : _size(0), _reserved(0), _data(nullptr) {
			}

			String(const CT* str, std::size_t length) : String() {
				if (length > 0) {
					_reserve<false>(length);
					std::memcpy(_data, str, length);
					_size = length;
					_zero_terminate();
				}
			}

			_RDX_INLINE explicit String(std::size_t rsv) : String() {
				_reserve<false>(rsv);
				_zero_terminate();
			}

			_RDX_INLINE String(const CT* str) : String(str, std::strlen(str)) {
			}

			//MOVE CTOR
			_RDX_INLINE String(String&& ref) : _data(ref._data), _reserved(ref._reserved), _size(ref._size) {
				ref._data = nullptr;
				ref._size = 0;
				ref._reserved = 0;
			}

			//MOVE ASSIGNMENT OP
			_RDX_INLINE String& operator=(String&& ref) {
				_data = ref._data;
				_reserved = ref._reserved;
				_size = ref._size;

				ref._data = nullptr;
				ref._size = 0;
				ref._reserved = 0;
				return *this;
			}

			//COPY CTOR
			_RDX_INLINE String(const String& ref) : String(ref._data, ref._size) {
			}
			
			//COPY ASSIGNMENT OP
			_RDX_INLINE String& operator=(const String& ref) {
				if (ref._size > 0) {
					_reserve<false>(ref._size);
					std::memcpy(_data, ref._data, ref._size);
					_size = ref._size;
					_zero_terminate();
				}
				else _size = 0;
				return *this;
			}

			_RDX_INLINE bool operator==(const String& ref) const {
				if (_size != ref._size)
					return false;
				return std::memcmp(_data, ref._data, _size) == 0;
			}

			_RDX_INLINE String& operator+=(const String& ref) {
				if (ref._size > 0) {
					_reserve<true>(_size + ref._size);
					std::memcpy(_data + _size, ref._data, ref._size);
					_size += ref._size;
					_zero_terminate();
				}
				return *this;
			}

			_RDX_INLINE ~String() {
				_dealloc();
			}

			_RDX_INLINE void reserve(std::size_t rsv) {
				_reserve<true>(rsv);
			}

			_RDX_INLINE String substr(std::size_t off, std::size_t size) const {
				return { _data + off, size };
			}

			_RDX_INLINE String substr(std::size_t off) const {
				return { _data + off, _size - off };
			}

			_RDX_INLINE CT& operator[](std::size_t index) {
				return _data[index];
			}

			_RDX_INLINE const CT& operator[](std::size_t index) const {
				return _data[index];
			}

			_RDX_INLINE std::size_t size() const {
				return _size;
			}

			_RDX_INLINE std::size_t capacity() const {
				return _reserved;
			}

			_RDX_INLINE bool empty() const {
				return _size == 0;
			}

			_RDX_INLINE const CT* cstr() const {
				if (_data == nullptr)
					//When the string is default-constructed i.e. empty 
					//we don't want to allocate memory just for 
					//zero-termination. Instead we use a small "sentinel" buffer
					return _empty;
				return _data;
			}

		private:
			template<bool Copy>
			_RDX_INLINE void _reserve(std::size_t rsv) {
				if (rsv > _reserved) {
					auto dest = Allocator::allocate(rsv + 1);
					if (Copy && !empty())
						std::memcpy(dest, _data, _size);
					_dealloc();
					_data = dest;
					_reserved = rsv;
				}
			}

			_RDX_INLINE void _dealloc() {
				Allocator::deallocate(_data);
			}

			_RDX_INLINE void _zero_terminate() {
				_data[_size] = '\0';
			}

			CT* _data;
			const CT _empty[1] = { '\0' };

			std::size_t _size;
			std::size_t _reserved;
		};
	}

	using String = detail::String<i8>;
}