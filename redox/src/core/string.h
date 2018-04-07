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
#include "non_copyable.h"

namespace redox {
	namespace detail {

		/**
		* FixedString
		* A fixed-size string container
		*/
		template<class CT, std::size_t N>
		class FixedString {
		public:
			using value_type_t = CT;
			using storage_type_t = typename std::aligned_storage<
				sizeof(value_type_t), alignof(value_type_t)>::type;

			/**
			* FixedString::FixedString()
			* Constructs FixedString (Empty)
			*/
			_RDX_INLINE FixedString() : _size(0) {
				_zero_terminate();
			}

			/**
			* FixedString::FixedString()
			* Constructs FixedString using a c_string and it's length
			*/
			FixedString(const value_type_t* str, std::size_t length) {
				if (length + 1 > N)
					throw std::bad_alloc();

				_size = length;
				for (size_t i = 0; i < _size; i++)
					at(i) = str[i];

				_zero_terminate();
			}

			/**
			* FixedString::FixedString()
			* Constructs FixedString using a zero-terminated string
			*/
			FixedString(const value_type_t* str) : FixedString(str, std::strlen(str)) {
			}

			/**
			* FixedString::~FixedString()
			* Destructs FixedString
			*/
			~FixedString() = default;

			/**
			* FixedString::size()
			* Returns the size of the stored string
			*/
			_RDX_INLINE std::size_t size() const {
				return _size;
			}

			/**
			* FixedString::operator[](size_t)
			* Returns string character at given index
			*/
			_RDX_INLINE value_type_t& operator[](const std::size_t index) {
				return at(index);
			}

			/**
			* FixedString::at(size_t)
			* Returns string character at given index
			*/
			_RDX_INLINE value_type_t& at(const std::size_t index) {
				return *reinterpret_cast<value_type_t*>(_data + index);
			}

			/**
			* FixedString::cstr()
			* Returns a cstring representation
			*/
			_RDX_INLINE const value_type_t* cstr() const {
				return _data;
			}

		private:
			_RDX_INLINE void _zero_terminate() {
				at(_size) = '\0';
			}

			storage_type_t _data[N];
			std::size_t _size;
		};

		/**
		* DynamicString
		* A dynamic-size string container
		*/
		template<class CT, class Allocator>
		class DynamicString {
		public:
			using value_type_t = CT;

			/**
			* DynamicString::DynamicString()
			* Constructs DynamicString (Empty)
			*/
			_RDX_INLINE DynamicString() : _size(0), _reserved(0), _data(nullptr) {
			}

			/**
			* DynamicString::DynamicString(str)
			* Constructs FixedString using a c_string and it's length
			*/
			DynamicString(const value_type_t* str, std::size_t length) : DynamicString() {
				reserve(length);
				_size = length;
				for (size_t i = 0; i < _size; i++)
					at(i) = str[i];

				if (_size > 0)
					_zero_terminate();
			}

			/**
			* DynamicString::DynamicString(str)
			* Constructs DynamicString using a zero-terminated string
			*/
			_RDX_INLINE DynamicString(const value_type_t* str) : DynamicString(str, std::strlen(str)) {
			}

			/**
			* DynamicString::DynamicString(&&)
			* Move constructor
			*/
			_RDX_INLINE DynamicString(DynamicString&& ref) : _data(ref._data), _reserved(ref._reserved), _size(ref._size) {
				ref._data = nullptr;

#ifdef RDX_DEBUG
				//Technically, moved-from objects are left in an undefined
				//state, so this is not exactly needed. But it improves debugging.
				ref._size = 0;
				ref._reserved = 0;
#endif
			}

			/**
			* DynamicString::operator=(&&)
			* Move assignment operator
			*/
			_RDX_INLINE DynamicString& operator=(DynamicString&& ref) {
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

			/**
			* DynamicString::DynamicString(const&)
			* Copy constructor
			*/
			_RDX_INLINE DynamicString(const DynamicString& ref) : DynamicString(ref._data, ref._size) {
			}

			/**
			* DynamicString::DynamicString(lhs, rhs)
			* Constructs DynamicString by merging two strings
			*/
			_RDX_INLINE DynamicString(const DynamicString& lhs, const DynamicString& rhs) {
				//TODO
			}

			/**
			* DynamicString::operator=(const&)
			* Copy assignment operator
			*/
			DynamicString& operator=(const DynamicString& ref) = delete;

			/**
			* DynamicString::operator+(const&)
			* Concatenates two strings
			*/
			_RDX_INLINE DynamicString operator+(const DynamicString& rhs) {
				return { *this, rhs };
			}

			/**
			* DynamicString::~DynamicString()
			* Destructs DynamicString
			*/
			_RDX_INLINE ~DynamicString() {
				_dealloc();
			}

			/**
			* DynamicString::reserve(size_t)
			* Reserves/allocates memory without initializing it
			*/
			_RDX_INLINE void reserve(const std::size_t size) {
				if (size > _reserved) {
					_dealloc();
					//todo:: realloc()
					_data = Allocator::allocate(size + 1);
					_reserved = size;
				}
			}

			/**
			* DynamicString::operator[](size_t)
			* Returns string character at given index
			*/
			_RDX_INLINE value_type_t& operator[](const std::size_t index) {
				return at(index);
			}

			/**
			* DynamicString::at(size_t)
			* Returns string character at given index
			*/
			_RDX_INLINE value_type_t& at(const std::size_t index) {
				return _data[index];
			}

			/**
			* DynamicString::size()
			* Returns the size of the stored string
			*/
			_RDX_INLINE std::size_t size() const {
				return _size;
			}

			/**
			* DynamicString::cstr()
			* Returns a cstring representation
			*/
			_RDX_INLINE const value_type_t* cstr() const {
				if (_data == nullptr)
					return _empty;
				return _data;
			}

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
	
	template<size_t N>
	using FixedString = detail::FixedString<i8, N>;

	using DynamicString = detail::DynamicString<i8, allocator::DefaultAllocator<i8>>;
}