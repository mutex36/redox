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
	template<class ValueType, 
		class Allocator = allocation::DefaultAllocator<ValueType>,
		class GrowthPolicy = allocation::DefaultGrowth>
	class Buffer : public NonCopyable {
	public:
		using size_type = std::size_t;

		Buffer() : _data(nullptr), _reserved(0), _size(0) {
		}

		_RDX_INLINE Buffer(size_type size) : Buffer() {
			resize(size);
		}

		_RDX_INLINE Buffer(std::initializer_list<ValueType> values) : Buffer() {
			reserve(values.size());

			//Unfortunately, initializer_list is a static container
			//that does not provide non-const access to it's data
			//This makes moving impossible and copying the only viable option.
			for (const auto& v : values)
				_push_no_checks(v);
		}

		_RDX_INLINE Buffer(Buffer&& ref) : _data(ref._data), _reserved(ref._reserved), _size(ref._size) {
			ref._data = nullptr;
			ref._size = 0;
			ref._reserved = 0;
		}

		_RDX_INLINE Buffer& operator=(Buffer&& ref) {
			_data = ref._data;
			_size = ref._size;
			_reserved = ref._reserved;
			ref._data = nullptr;
			ref._size = 0;
			ref._reserved = 0;
			return *this;
		}

		template<class _T>
		void push(_T&& element) {
			_realloc_check();
			_push_no_checks(std::forward<_T>(element));
		}

		_RDX_INLINE ~Buffer() {
			if constexpr(!std::is_trivially_destructible_v<ValueType>)
				for (auto& c : *this) c.~ValueType();

			_dealloc();
		}

		_RDX_INLINE ValueType& operator[](size_type index) {
			if (index >= _size)
				throw Exception("index out of bounds");
			return _data[index];
		}

		_RDX_INLINE const ValueType& operator[](size_type index) const {
			if (index >= _size)
				throw Exception("index out of bounds");
			return _data[index];
		}

		void reserve(size_type size) {
			if (size > _reserved) {
				auto dest = Allocator::allocate(size);
				if (!empty()) {
					for (size_type elm = 0; elm < _size; ++elm)
						new (dest + elm) ValueType(std::move(_data[elm]));
				}
				_dealloc();
				_data = dest;
				_reserved = size;
			}
		}

		void resize(size_type size) {
			static_assert(std::is_default_constructible_v<ValueType>);
			if (size > _size) {
				reserve(size);
				for (size_type elm = _size; elm < size; ++elm)
					new (_data + elm) ValueType();
				_size = size;
			}
		}

		_RDX_INLINE ValueType* data() const {
			return _data;
		}

		_RDX_INLINE size_type size() const {
			return _size;
		}

		_RDX_INLINE size_type empty() const {
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
			new (_data + _size++) ValueType(std::forward<_T>(element));
		}

		_RDX_INLINE void _dealloc() {
			Allocator::deallocate(_data);
		}

		_RDX_INLINE bool _full() {
			return _size == _reserved;
		}

		size_type _reserved;
		size_type _size;
		ValueType* _data;
	};
}