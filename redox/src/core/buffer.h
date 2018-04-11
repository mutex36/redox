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
#include "core\allocator\default_alloc.h"

#include <initializer_list>
#include <type_traits> //std::move()
#include <new> //placement-new

namespace redox {

	template<class T, class Allocator = DefaultAllocator<T>>
	class Buffer {
	public:
		static constexpr f32 kGrowthFactor = 2.0f;

		_RDX_INLINE Buffer() : _data(nullptr), _reserved(0), _size(0) {
		}

		_RDX_INLINE Buffer(const std::size_t size) : Buffer() {
			reserve(size);
		}

		_RDX_INLINE Buffer(std::initializer_list<T> values) : Buffer(values.size()) {
			for (auto& v : values) {
				_push_no_checks(v);
			}
		}

		void push(const T& element) {
			if (_full()) {
				//It makes little sense to grow the vector by just one element
				//Doubling (GrowthFactor) the size is likely more efficient
				//as it reduces expensive reallocations/copies
				reserve(static_cast<std::size_t>((_size + 1) * kGrowthFactor));
			}
			_push_no_checks(element);
		}

		_RDX_INLINE ~Buffer() {
			_dealloc();
		}

		_RDX_INLINE T& operator[](const std::size_t index) {
			return at(index);
		}

		_RDX_INLINE T& at(const std::size_t index) {
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

		template<class...Args>
		_RDX_INLINE void construct(Args&&...args) {
			for (std::size_t elm = 0; elm < _reserved; ++elm)
				new (_data + elm) T(std::forward<Args>(args)...);
			_size = _reserved;
		}

		_RDX_INLINE std::size_t size() const {
			return _size;
		}

		_RDX_INLINE std::size_t empty() const {
			return _size == 0;
		}

		auto begin() const {
			return _data; 
		}

		auto end() const {
			return _data + _size;
		}

	private:
		_RDX_INLINE void _push_no_checks(const T& element) {
			new (_data + _size++) T(element);
		}

		_RDX_INLINE void _dealloc() {
			for (std::size_t i = 0; i < _size; i++)
				_data[i].~T();
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