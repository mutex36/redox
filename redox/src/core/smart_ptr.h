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

#ifndef RDX_DECL_CONSTRUCT_TAG
#define RDX_DECL_CONSTRUCT_TAG
	struct construct_tag {};
#endif

	template<class T,
		class Allocator = allocation::DefaultAllocator<T>>
	class SmartPtr : public NonCopyable {
	public:
		SmartPtr() : _raw(nullptr) {}

		template<class...Args>
		_RDX_INLINE SmartPtr(construct_tag, Args&&...args) {
			_raw = new (Allocator::allocate()) T(std::forward<Args>(args)...);
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

		_RDX_INLINE auto operator->() const {
			return _raw;
		}

		_RDX_INLINE auto get() const {
			return _raw;
		}

	private:
		T* _raw;
	};
}