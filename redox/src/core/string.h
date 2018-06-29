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

	class StringView {
	public:
		using size_type = std::size_t;
		using char_type = char;

		StringView() : _data(nullptr), _size(0) {}

		//template<size_type Length>
		//StringView(const char_type(&literal)[Length]) :
		//	_data(literal), _size(Length - 1) {}

		StringView(const char_type* str) :
			_data(str), _size(std::strlen(str)) {}

		StringView(const char_type* data, size_type size) :
			_data(data), _size(size) {}

		_RDX_INLINE size_type size() const {
			return _size;
		}

		_RDX_INLINE const char_type* cstr() const {
			return _data;
		}

		_RDX_INLINE const char_type* data() const {
			return _data;
		}

		_RDX_INLINE bool empty() const {
			return _size == 0;
		}

		_RDX_INLINE const char_type& operator[](size_type index) const {
			return _data[index];
		}

		_RDX_INLINE bool operator==(const StringView& ref) const {
			if (_size != ref._size)
				return false;
			return std::memcmp(_data, ref._data, _size) == 0;
		}

		_RDX_INLINE StringView substr(size_type off, size_type size) const {
			return { _data + off, size };
		}

		_RDX_INLINE StringView substr(size_type off) const {
			return { _data + off, _size - off };
		}

	private:
		const size_type  _size;
		const char_type* _data;
	};

	template<class Allocator>
	class BaseString {
	public:
		using size_type = std::size_t;
		using char_type = char;
		static constexpr char_type zero_terminator = '\0';

		BaseString() : _size(0), _reserved(0), _data(nullptr) {}

		_RDX_INLINE BaseString(const char_type* str, size_type length) : BaseString() {
			_reserve<copy_policy::no_copy>(length);
			_append(str, length);
			_zero_terminate();
		}

		_RDX_INLINE BaseString(const StringView& view) 
			: BaseString(view.cstr(), view.size()) {}

		_RDX_INLINE explicit BaseString(size_type reserve) : BaseString() {
			_reserve<copy_policy::no_copy>(reserve);
			_zero_terminate();
		}

		//template<size_type Length>
		//BaseString(const char_type(&literal)[Length]) :
		//	BaseString(literal, Length - 1) {}

		BaseString(const char_type* str) :
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
			_reserve<copy_policy::copy>(reserve);
		}

		_RDX_INLINE BaseString& operator=(const BaseString& ref) {
			_reserve<copy_policy::no_copy>(ref._size);
			_append(ref._data, ref._size);
			_zero_terminate();
			return *this;
		}

		_RDX_INLINE BaseString& operator+=(const StringView& ref) {
			_reserve<copy_policy::copy>(_size + ref.size());
			_append(ref.data(), ref.size());
			_zero_terminate();
			return *this;
		}

		_RDX_INLINE BaseString operator+(const StringView& ref) const {
			BaseString out(_size + ref.size());
			out._append(_data, _size);
			out._append(ref.data(), ref.size());
			out._zero_terminate();
			return out;
		}

		_RDX_INLINE bool operator==(const BaseString& ref) const {
			if (_size != ref._size)
				return false;
			return std::memcmp(_data, ref._data, _size) == 0;
		}

		_RDX_INLINE bool operator!=(const BaseString& ref) const {
			if (_size != ref._size)
				return true;
			return std::memcmp(_data, ref._data, _size) != 0;
		}

		_RDX_INLINE operator StringView() const {
			return { _data, _size };
		}

		_RDX_INLINE StringView substr(size_type off, size_type size) const {
			return { _data + off, size };
		}

		_RDX_INLINE StringView substr(size_type off) const {
			return { _data + off, _size - off };
		}

		_RDX_INLINE char_type& operator[](size_type index) {
			if (index >= _size)
				throw Exception("index out of bounds");
			return _data[index];
		}

		_RDX_INLINE const char_type& operator[](size_type index) const {
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

		_RDX_INLINE const char_type* cstr() const {
			if (_data == nullptr) 
				return &zero_terminator;
			return _data;
		}

	private:
		_RDX_INLINE void _append(const char_type* str, size_type length) {
			std::memcpy(_data + _size, str, length);
			_size += length;
		}

		_RDX_INLINE void _dealloc() const {
			Allocator::deallocate(_data);
		}

		_RDX_INLINE void _zero_terminate() {
			if (_size > 0)
				_data[_size] = zero_terminator;
		}

		struct copy_policy {
			struct copy;
			struct no_copy;
		};

		template<class Copy>
		void _reserve(size_type reserve) {
			if (reserve <= _reserved)
				return;

			auto buffer = Allocator::allocate(reserve + 1); //+1 null-term
			if constexpr(std::is_same_v<Copy, copy_policy::copy>)
				std::memcpy(buffer, _data, _size);
			else _size = 0;

			_dealloc();
			_data = buffer;
			_reserved = reserve;
		}

		char_type* _data;
		size_type _size;
		size_type _reserved;
	};

	using String = BaseString<allocation::DefaultAllocator<char>>;
}