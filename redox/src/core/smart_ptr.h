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
#include "core\non_copyable.h"
#include "core\allocation\default_allocator.h"

#include <type_traits> //std::forward

namespace redox {

	template<class T,
		class Allocator = allocation::DefaultAllocator<T>>
	class SmartPtr : public NonCopyable {
	public:
		using ptr_type = T*;

		SmartPtr() : _raw(nullptr) {
		}
		SmartPtr(ptr_type ptr) : _raw(ptr) {
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

		_RDX_INLINE ptr_type operator->() const {
			return _raw;
		}

		_RDX_INLINE ptr_type get() const {
			return _raw;
		}

	private:
		ptr_type _raw;
	};

	template<class T, class Allocator = allocation::DefaultAllocator<T>, class...Args>
	SmartPtr<T, Allocator> make_smart_ptr(Args&&...args) {
		return { new (Allocator::allocate()) T(std::forward<Args>(args)...) };
	}
}