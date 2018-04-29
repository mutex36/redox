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

namespace redox {
	namespace detail { using refcount_t = std::size_t; }

	template<class T,
		class Allocator = allocation::DefaultAllocator<T>,
		class RefCountAllocator = typename Allocator::template rebind<detail::refcount_t>>
	class RefCounted {
	public:
		using ptr_type = T*;

		RefCounted() : _data(nullptr), _refcount(nullptr) {
		}

		_RDX_INLINE RefCounted(ptr_type ptr) : _data(ptr) {
			_init_ref_count();
		}

		_RDX_INLINE ~RefCounted() {
			if (_refcount && _dec_ref_count() == 0) {
				Allocator::deallocate(_data);
				RefCountAllocator::deallocate(_refcount);
			}
		}

		_RDX_INLINE RefCounted(const RefCounted& ref) : _data(ref._data), _refcount(ref._refcount) {
			_inc_ref_count();
		}

		RefCounted(RefCounted&& ref) : _data(ref._data), _refcount(ref._refcount) {
			ref._data = nullptr;
			ref._refcount = nullptr;
		}

		_RDX_INLINE RefCounted& operator=(const RefCounted& ref) {
			_data = ref._data;
			_refcount = ref._refcount;
			_inc_ref_count();
			return *this;
		}

		_RDX_INLINE RefCounted& operator=(RefCounted&& ref) {
			_data = ref._data;
			_refcount = ref._refcount;
			ref._data = nullptr;
			ref._refcount = nullptr;
			return *this;
		}

		_RDX_INLINE ptr_type operator->() const {
			return _data;
		}

		_RDX_INLINE T& operator*() const {
			return *_data;
		}

		_RDX_INLINE ptr_type get() const {
			return _data;
		}

		_RDX_INLINE detail::refcount_t ref_count() const {
			return *_refcount;
		}

	protected:
		_RDX_INLINE auto _dec_ref_count() {
			//returns new ref_count
			return --(*_refcount);
		}

		_RDX_INLINE void _inc_ref_count() {
			(*_refcount)++;
		}

		_RDX_INLINE void _init_ref_count() {
			_refcount = RefCountAllocator::allocate();
			*_refcount = 1;
		}

		ptr_type _data;
		detail::refcount_t* _refcount;
	};

	//TODO: combined data and control block allocation

	template<class T,
		class Allocator = allocation::DefaultAllocator<T>,
		class RefCountAllocator = typename Allocator::template rebind<detail::refcount_t>, class...Args>
	_RDX_INLINE RefCounted<T, Allocator, RefCountAllocator> make_ref_counted(Args&&...args) {
		return new (Allocator::allocate()) T(std::forward<Args>(args)...);
	}
}