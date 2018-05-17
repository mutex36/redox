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
#include "vec.h"
#include "simd.h"

namespace redox::math {
	template<class Scalar, class XMM>
	struct Mat44 {
		using vec3_type = Vec<Scalar, XMM, 3>;
		using vec4_type = Vec<Scalar, XMM, 4>;

		Mat44() : _xmm{ simd::set_zero(), simd::set_zero(),
			simd::set_zero(), simd::set_zero() } {
		}

		Mat44(XMM xmm0, XMM xmm1, XMM xmm2, XMM xmm3)
			: _xmm{ xmm0, xmm1, xmm2, xmm3 } {
		}

		_RDX_INLINE Mat44 operator+(const Mat44& rhs) const {
			return { 
				simd::add(_xmm[0], rhs._xmm[0]), 
				simd::add(_xmm[1], rhs._xmm[1]),
				simd::add(_xmm[2], rhs._xmm[2]),
				simd::add(_xmm[3], rhs._xmm[3])
			};
		}

		_RDX_INLINE static Mat44 identity() {
			return {
				simd::set(1,0,0,0),
				simd::set(0,1,0,0),
				simd::set(0,0,1,0),
				simd::set(0,0,0,1)
			};
		}

		_RDX_INLINE static Mat44 scale(const vec3_type& scale) {
			return {
				simd::blend<0x1>(simd::set_zero(), scale._xmm),
				simd::blend<0x2>(simd::set_zero(), scale._xmm),
				simd::blend<0x4>(simd::set_zero(), scale._xmm),
				simd::set(0,0,0,1)
			};
		}

		//_RDX_INLINE static Mat44 translate(const vec3_type& pos) {
		//	return {
		//		simd::set(1,0,0,0), scale._xmm), //Simd::set(1,0,0,pos.x),
		//		simd::set(0,1,0,0), scale._xmm),
		//		Simd::set(0,0,1,pos.z),
		//		Simd::set(0,0,0,1)
		//	};
		//}

		constexpr vec4_type operator[](std::size_t index) {
			return _xmm[index];
		}

		XMM _xmm[4];
	};

	using Mat44f = Mat44<f32, simd::f32x4>;
}