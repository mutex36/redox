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

	template<class CharType, class Allocator>
	class BaseString {
	public:
		using size_type = std::size_t;
		static constexpr CharType zero_terminator = '\0';

		BaseString() : _size(0), _reserved(0), _data(nullptr) {}

		_RDX_INLINE BaseString(const CharType* str, size_type length) : BaseString() {
			_reserve_checks<copy_policy::no_copy>(length);
			_append_no_checks(str, length);
			_zero_terminate();
		}

		_RDX_INLINE explicit BaseString(size_type reserve) : BaseString() {
			_reserve_checks<copy_policy::no_copy>(reserve);
			_zero_terminate();
		}

		template<size_type Length>
		BaseString(const CharType(&literal)[Length]) :
			BaseString(literal, Length) {}

		BaseString(const CharType* str) :
			BaseString(str, std::strlen(str)) {}

		BaseString(const BaseString& ref) :
			BaseString(ref._data, ref._size) {}

		_RDX_INLINE BaseString(BaseString&& ref) : 
			_data(ref._data),
			_reserved(ref._reserved),
			_size(ref._size) {
			ref._data = nullptr;
			ref._size = 0;
			ref._reserved = 0;
		}

		_RDX_INLINE BaseString& operator=(BaseString&& ref) {
			_data = ref._data;
			_reserved = ref._reserved;
			_size = ref._size;

			ref._data = nullptr;
			ref._size = 0;
			ref._reserved = 0;
			return *this;
		}

		_RDX_INLINE ~BaseString() {
			_dealloc();
		}

		_RDX_INLINE void reserve(size_type reserve) {
			_reserve_checks<copy_policy::copy>(reserve);
		}

		_RDX_INLINE BaseString& operator=(const BaseString& ref) {
			_reserve_checks<copy_policy::no_copy>(ref._size);
			_append_no_checks(ref._data, ref._size);
			_zero_terminate();
			return *this;
		}

		_RDX_INLINE BaseString& operator+=(const BaseString& ref) {
			_reserve_checks<copy_policy::copy>(_size + ref._size);
			_append_no_checks(ref._data, ref._size);
			_zero_terminate();
			return *this;
		}

		BaseString operator+(const BaseString& ref) const {
			BaseString out(_size + ref._size);
			out._append_no_checks(_data, _size);
			out._append_no_checks(ref._data, ref._size);
			out._zero_terminate();
			return out;
		}

		_RDX_INLINE bool operator==(const BaseString& ref) const {
			if (_size != ref._size)
				return false;
			return std::memcmp(_data, ref._data, _size) == 0;
		}

		_RDX_INLINE BaseString substr(size_type off, size_type size) const {
			return { _data + off, size };
		}

		_RDX_INLINE BaseString substr(size_type off) const {
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

		_RDX_INLINE void clear() {
			_size = 0;
		}

		_RDX_INLINE size_type capacity() const {
			return _reserved;
		}

		_RDX_INLINE bool empty() const {
			return _size == 0;
		}

		_RDX_INLINE const CharType* cstr() const {
			if (_data == nullptr) 
				return &zero_terminator;
			return _data;
		}

	private:
		_RDX_INLINE void _append_no_checks(const CharType* str, size_type length) {
			std::memcpy(_data + _size, str, length);
			_size += length;
		}

		struct copy_policy {
			struct copy;
			struct no_copy;
		};

		template<class Copy>
		_RDX_INLINE void _reserve_checks(size_type reserve) {
			if (reserve > _reserved)
				_reserve_no_checks<Copy>(reserve);
		}

		template<class Copy>
		void _reserve_no_checks(size_type reserve) {
			auto buffer = _alloc(reserve);

			//Sometimes it's not needed to transfer old string data
			//into reallocated buffer
			if constexpr(std::is_same_v<Copy, copy_policy::copy>) {
				//transfer data into allocated buffer
				std::memcpy(buffer, _data, _size);
			} else {
				//we did not transfer data, the string is now empty
				_size = 0; 
			}

			_dealloc();
			_data = buffer;
			_reserved = reserve;
		}

		_RDX_INLINE void _dealloc() const{
			Allocator::deallocate(_data);
		}

		_RDX_INLINE auto _alloc(size_type size) const {
			return Allocator::allocate(size + 1); //+1 null-term
		}

		_RDX_INLINE void _zero_terminate() {
			if (_size > 0)
				_data[_size] = zero_terminator;
		}

		CharType* _data;
		size_type _size;
		size_type _reserved;
	};

	using String = BaseString<i8, allocation::DefaultAllocator<i8>>;
}