#pragma once
#include "core\core.h"
#include "core\non_copyable.h"
#include "core\allocator\default_alloc.h"

#include <type_traits> //std::forward

namespace redox {

	/**
	* SmartPtr
	* A move-only smart pointer
	*/
	template<class T, class Allocator = allocator::DefaultAllocator<T>>
	class SmartPtr : public NonCopyable {
	public:
		/**
		* SmartPtr::SmartPtr()
		* Constructor (Empty)
		*/
		SmartPtr() : _raw(nullptr) {
		}

		/**
		* SmartPtr::SmartPtr(T*)
		* Constructs SmartPtr using a raw pointer
		*/
		SmartPtr(T* raw) : _raw(raw) {
		}

		/**
		* SmartPtr::SmartPtr(&&)
		* Move constructor
		*/
		SmartPtr(SmartPtr&& ref) : _raw(ref._raw) {
			ref._raw = nullptr;
		}

		/**
		* SmartPtr::operator=(&&)
		* Copy assignment operator
		*/
		SmartPtr& operator=(SmartPtr&& ref) {
			_raw = ref._raw;
			ref._raw = nullptr;
			return *this;
		}

		/**
		* SmartPtr::~SmartPtr()
		* Destructor
		*/
		~SmartPtr() {
			Allocator::deallocate(_raw);
		}

		/**
		* SmartPtr::operator->()
		* Arrow operator.
		*/
		T* operator->() {
			return _raw;
		}

	private:
		T* _raw;
	};

	template<class T, class Allocator = allocator::DefaultAllocator<T>, class...Args>
	_RDX_INLINE SmartPtr<T, Allocator> make_smart_ptr(Args&&...args) {
		return { new (Allocator::allocate()) T(std::forward<Args>(args)...) };
	}
}