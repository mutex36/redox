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
#include "core\allocation\default_allocator.h"
#include "core\allocation\growth_policy.h"

#include <initializer_list>
#include <type_traits> //std::move
#include <new> //placement-new
#include <algorithm> //std::max

namespace redox {
	template<class T, 
		class Allocator = allocation::DefaultAllocator<T>,
		class GrowthPolicy = allocation::DefaultGrowth>
	class Buffer {
	public:
		_RDX_INLINE Buffer() : _data(nullptr), _reserved(0), _size(0) {
		}

		_RDX_INLINE Buffer(std::size_t size) : Buffer() {
			resize(size);
		}

		_RDX_INLINE Buffer(std::initializer_list<T> values) : Buffer() {
			reserve(values.size());

			//Unfortunately, initializer_list is a static container
			//that does not provide non-const access to it's data
			//This makes moving impossible and copying the only viable option.
			for (auto& v : values)
				_push_no_checks(v);
		}

		template<class _T>
		void push(_T&& element) {
			_realloc_check();
			_push_no_checks(std::forward<_T>(element));
		}

		_RDX_INLINE ~Buffer() {
			for (auto& c : *this) c.~T();
			_dealloc();
		}

		_RDX_INLINE T& operator[](std::size_t index) {
			if (index >= _size)
				throw Exception("index out of bounds");
			return _data[index];
		}

		_RDX_INLINE const T& operator[](std::size_t index) const {
			if (index >= _size)
				throw Exception("index out of bounds");
			return _data[index];
		}

		void reserve(const std::size_t size) {
			if (size > _reserved) {
				auto dest = Allocator::allocate(size);
				if (!empty()) {
					for (std::size_t elm = 0; elm < _size; ++elm)
						new (dest + elm) T(std::move(_data[elm]));
				}
				_dealloc();
				_data = dest;
				_reserved = size;
			}
		}

		void resize(const std::size_t size) {
			static_assert(std::is_default_constructible_v<T>);
			if (size > _size) {
				reserve(size);
				for (std::size_t elm = _size; elm < size; ++elm)
					new (_data + elm) T();
				_size = size;
			}
		}

		_RDX_INLINE T* data() {
			return _data;
		}

		_RDX_INLINE std::size_t size() const {
			return _size;
		}

		_RDX_INLINE std::size_t empty() const {
			return _size == 0;
		}

		_RDX_INLINE auto begin() const {
			return _data;
		}

		_RDX_INLINE auto end() const {
			return _data + _size;
		}

	private:
		_RDX_INLINE void _realloc_check() {
			if (_full()) {
				//It makes little sense to grow the vector by just one element
				//Doubling (GrowthPolicy) the size is likely more efficient
				//as it reduces expensive reallocations/copies
				GrowthPolicy fn;
				reserve(fn(_size));
			}
		}

		template<class _T>
		_RDX_INLINE void _push_no_checks(_T&& element) {
			new (_data + _size++) T(std::forward<_T>(element));
		}

		_RDX_INLINE void _dealloc() {
			Allocator::deallocate(_data);
		}

		_RDX_INLINE bool _full() {
			return _size == _reserved;
		}

		std::size_t _reserved;
		std::size_t _size;
		T* _data;
	};
}