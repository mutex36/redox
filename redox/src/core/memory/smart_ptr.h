#pragma once
#include "core\core.h"
#include "core\non_copyable.h"
#include "core\allocator\default_alloc.h"

#include <type_traits> //std::forward

namespace redox {

	template<class T, class Allocator = DefaultAllocator<T>>
	class SmartPtr : public NonCopyable {
	public:
		using ptr_t = T*;

		_RDX_INLINE SmartPtr() : _raw(nullptr) {
		}

		_RDX_INLINE SmartPtr(ptr_t raw) : _raw(raw) {
		}

		_RDX_INLINE SmartPtr(SmartPtr&& ref) : _raw(ref._raw) {
			ref._raw = nullptr;
		}

		_RDX_INLINE SmartPtr& operator=(SmartPtr&& ref) {
			_raw = ref._raw;
			ref._raw = nullptr;
			return *this;
		}

		_RDX_INLINE ~SmartPtr() {
			Allocator::deallocate(_raw);
		}

		_RDX_INLINE ptr_t operator->() {
			return _raw;
		}

		_RDX_INLINE ptr_t get() {
			return _raw;
		}

	private:
		ptr_t _raw;
	};

	template<class T, class Allocator = DefaultAllocator<T>, class...Args>
	_RDX_INLINE SmartPtr<T, Allocator> make_smart_ptr(Args&&...args) {
		return { new (Allocator::allocate()) T(std::forward<Args>(args)...) };
	}
}