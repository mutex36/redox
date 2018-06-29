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

#ifdef RDX_COMPILER_MSVC
#include <intrin.h>
#elif defined RDX_COMPILER_GCC || RDX_COMPILER_CLANG
#include <x86intrin.h>
#endif

namespace redox::simd {
	typedef __m128  f32x4;
	typedef __m128d f64x4;
	typedef __m128i i32x4;

	constexpr std::size_t alignment = 16;

	template<class Scalar, class XMM, u32 Index>
	struct accessor {
		_RDX_INLINE operator Scalar() const noexcept {
			return extract_by_index<Index>(_xmm);
		}

		_RDX_INLINE accessor& operator=(const Scalar value) noexcept {
			_xmm = set_by_index<Index>(value, _xmm);
			return *this;
		}

		XMM _xmm;
	};

	template<u32 Index, class XMM>
	_RDX_INLINE auto extract_by_index(XMM xmm) {
		return extract_lower(swizzle1<Index>(xmm));
	}

	template<u32 Index, class Scalar, class XMM>
	_RDX_INLINE auto set_by_index(const Scalar value, XMM xmm) {
		return blend<(0x1 << Index)>(xmm, set_all(value));
	}

	template<u32 imm8>
	_RDX_INLINE f32x4 blend(f32x4 lhs, f32x4 rhs) {
		//r0 : = (mask0 == 0) ? a0 : b0
		//r1 : = (mask1 == 0) ? a1 : b1
		//r2 : = (mask2 == 0) ? a2 : b2
		//r3 : = (mask3 == 0) ? a3 : b3
		return _mm_blend_ps(lhs, rhs, imm8);
	}

	_RDX_INLINE f32x4 add(f32x4 lhs, f32x4 rhs) {
		return _mm_add_ps(lhs, rhs);
	}
	_RDX_INLINE f32x4 sub(f32x4 lhs, f32x4 rhs) {
		return _mm_sub_ps(lhs, rhs);
	}
	_RDX_INLINE f32x4 mul(f32x4 lhs, f32x4 rhs) {
		return _mm_mul_ps(lhs, rhs);
	}
	_RDX_INLINE f32x4 div(f32x4 lhs, f32x4 rhs) {
		return _mm_div_ps(lhs, rhs);
	}

	template<i32 mask>
	_RDX_INLINE f32x4 dot(f32x4 lhs, f32x4 rhs) {
		return _mm_dp_ps(lhs, rhs, mask);
	}
	_RDX_INLINE f32x4 rsqrt(f32x4 xmm) {
		return _mm_rsqrt_ps(xmm);
	}

	_RDX_INLINE f32x4 set_zero() {
		return _mm_setzero_ps();
	}
	_RDX_INLINE f32x4 set_all(f32 x) {
		return _mm_set1_ps(x);
	}
	_RDX_INLINE f32x4 set(f32 x, f32 y = 0.0f, f32 z = 0.0f, f32 w = 0.0f) {
		return _mm_set_ps(w, z, y, x);
	}
	_RDX_INLINE f32x4 set_lower(f32 w) {
		return _mm_set_ss(w); //really just no-op/cast
	}

	template<u32 i1, u32 i2, u32 i3, u32 i4>
	_RDX_INLINE f32x4 shuffle(f32x4 a, f32x4 b) {
		return _mm_shuffle_ps(a, b, _MM_SHUFFLE(i1, i2, i3, i4));
	}

	template<u32 i1, u32 i2, u32 i3, u32 i4>
	_RDX_INLINE f32x4 swizzle(f32x4 a) {
		return shuffle<i1, i2, i3, i4>(a, a);
	}

	template<u32 i1>
	_RDX_INLINE f32x4 swizzle1(f32x4 a) {
		return swizzle<i1, i1, i1, i1>(a);
	}

	_RDX_INLINE f32x4 move_lower(f32x4 lhs, f32x4 rhs) {
		return _mm_move_ss(lhs, rhs);
	}
	_RDX_INLINE f32 extract_lower(f32x4 xmm) {
		return _mm_cvtss_f32(xmm);
	}
	_RDX_INLINE f32x4 sqrt_lower(f32x4 xmm) {
		return _mm_sqrt_ss(xmm);
	}
	_RDX_INLINE f32x4 rsqrt_lower(f32x4 xmm) {
		return _mm_rsqrt_ss(xmm);
	}
}