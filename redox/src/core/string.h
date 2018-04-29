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
		template<class CharType, class Allocator>
		class String {
		public:
			using size_type = std::size_t;

			_RDX_INLINE String() : _size(0), _reserved(0), _data(nullptr) {
			}

			String(const CharType* str, size_type length) : String() {
				if (length > 0) {
					_reserve<false>(length);
					std::memcpy(_data, str, length);
					_size = length;
					_zero_terminate();
				}
			}

			_RDX_INLINE explicit String(size_type rsv) : String() {
				_reserve<false>(rsv);
				_zero_terminate();
			}

			template<size_type Length>
			String(const CharType(&literal)[Length]) : String(literal, Length){
			}

			String(const CharType* str) : String(str, std::strlen(str)) {
			}

			//MOVE CTOR
			_RDX_INLINE String(String&& ref)  
				: _data(ref._data), _reserved(ref._reserved), _size(ref._size) {
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
			String& operator=(const String& ref) {
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
					_append_no_checks(ref);
					_zero_terminate();
				}
				return *this;
			}

			String operator+(const String& ref) {
				String out(_size + ref._size);
				out._append_no_checks(*this);
				out._append_no_checks(ref);
				out._zero_terminate();
				return out;
			}

			_RDX_INLINE ~String() {
				_dealloc();
			}

			_RDX_INLINE void reserve(size_type rsv) {
				_reserve<true>(rsv);
			}

			_RDX_INLINE String substr(size_type off, size_type size) const {
				return { _data + off, size };
			}

			_RDX_INLINE String substr(size_type off) const {
				return { _data + off, _size - off };
			}

			_RDX_INLINE CharType& operator[](size_type index) {
				if (index >= _size)
					throw Exception("index out of bounds");
				return _data[index];
			}

			_RDX_INLINE const CharType& operator[](size_type index) const {
				if (index >= _size)
					throw Exception("index out of bounds");
				return _data[index];
			}

			_RDX_INLINE size_type size() const {
				return _size;
			}

			_RDX_INLINE size_type capacity() const {
				return _reserved;
			}

			_RDX_INLINE bool empty() const {
				return _size == 0;
			}

			_RDX_INLINE const CharType* cstr() const {
				if (_data == nullptr)
					//When the string is default-constructed i.e. empty 
					//we don't want to allocate memory just for 
					//zero-termination. Instead we use a small "sentinel" buffer
					return &_empty;
				return _data;
			}

		private:
			_RDX_INLINE void _append_no_checks(const String& ref) {
				std::memcpy(_data + _size, ref._data, ref._size);
				_size += ref._size;
			}

			template<bool Copy>
			void _reserve(size_type rsv) {
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

			CharType* _data;
			const CharType _empty = '\0';

			size_type _size;
			size_type _reserved;
		};
	}

	using String = detail::String<i8, allocation::DefaultAllocator<i8>>;
}