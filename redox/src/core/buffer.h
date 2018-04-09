#pragma once
#include "core\core.h"
#include "core\allocator\default_alloc.h"

#include <initializer_list>
#include <type_traits> //std::move()

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

		_RDX_INLINE void push(const T& element) {
			if (_full()) {
				//It makes little sense to grow the vector by just one element
				//Doubling (GrowthFactor) the size is likely more efficient
				//as it reduces expensive reallocations/copies
				reserve(static_cast<std::size_t>((_size + 1) * kGrowthFactor));
			}

			new (_data + _size++) T(element);
		}

		_RDX_INLINE ~Buffer() {
			_dealloc();
		}

		_RDX_INLINE T& operator[](const std::size_t index) {
			return _data[index];
		}

		void reserve(const std::size_t size) {
			if (size > _reserved) {
				auto chk = Allocator::allocate(size);
				if (_data != nullptr) {
					//copy/move data from old to new memory
					for (std::size_t elm = 0; elm < _size; ++elm)
						new (chk + elm) T(std::move(_data[elm]));
				}
				_dealloc();
				_data = chk;
				_reserved = size;
			}
		}

		_RDX_INLINE std::size_t size() const {
			return _size;
		}

	private:
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