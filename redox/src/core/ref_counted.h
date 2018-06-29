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
	namespace detail { 
		using refcount_type = std::size_t; 

		template<class T, class RefCountType>
		struct control_block {
			RefCountType refcount;
			T data;
		};
	}

#ifndef RDX_DECL_CONSTRUCT_TAG
#define RDX_DECL_CONSTRUCT_TAG
	struct construct_tag {};
#endif

	template<class T, 
		class Allocator = allocation::DefaultAllocator<
		detail::control_block<T, detail::refcount_type>>>
	class RefCounted {
	public:
		using control_block_type = detail::control_block<T, detail::refcount_type>;

		RefCounted() : _cb(nullptr) {}

		template<class...Args>
		_RDX_INLINE RefCounted(construct_tag, Args&&...args) : _cb(Allocator::allocate()) {
			new (get()) T(std::forward<Args>(args)...);
			_init_ref_count();
		}

		_RDX_INLINE ~RefCounted() {
			if (_cb && _dec_ref_count() == 0) {
				_cb->data.~T();
				Allocator::deallocate(_cb);
			}
		}

		_RDX_INLINE RefCounted(const RefCounted& ref) : _cb(ref._cb) {
			_inc_ref_count();
		}

		_RDX_INLINE RefCounted(RefCounted&& ref) : _cb(ref._cb) {
			ref._cb = nullptr;
		}

		_RDX_INLINE RefCounted& operator=(const RefCounted& ref) {
			_cb = ref._cb;
			_inc_ref_count();
			return *this;
		}

		_RDX_INLINE RefCounted& operator=(RefCounted&& ref) {
			_cb = ref._cb;
			ref._cb = nullptr;
			return *this;
		}

		_RDX_INLINE operator bool() const {
			return _cb != nullptr;
		}

		_RDX_INLINE auto operator->() const{
			return get();
		}

		_RDX_INLINE auto& operator*() const {
			return _cb->data;
		}

		_RDX_INLINE auto get() const {
			return std::addressof(_cb->data);
		}

		_RDX_INLINE auto ref_count() const {
			return _cb->refcount;
		}

	protected:
		_RDX_INLINE auto _dec_ref_count() {
			return --(_cb->refcount);
		}

		_RDX_INLINE void _inc_ref_count() {
			(_cb->refcount)++;
		}

		_RDX_INLINE void _init_ref_count() {
			_cb->refcount = 1;
		}

		control_block_type* _cb;
	};
}