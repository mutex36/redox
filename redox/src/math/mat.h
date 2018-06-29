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

#include <array> //std::array

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

		Mat44(const std::array<Scalar, 16>& values) : Mat44(
			simd::set(values[0],  values[1],  values[2],  values[3]),
			simd::set(values[4],  values[5],  values[6],  values[7]),
			simd::set(values[8],  values[9],  values[10], values[11]),
			simd::set(values[12], values[13], values[14], values[15])) {

		}

		_RDX_INLINE Mat44 operator+(const Mat44& rhs) const {
			return { 
				simd::add(_xmm[0], rhs._xmm[0]), 
				simd::add(_xmm[1], rhs._xmm[1]),
				simd::add(_xmm[2], rhs._xmm[2]),
				simd::add(_xmm[3], rhs._xmm[3])
			};
		}

		//_RDX_INLINE Mat44 inverse() const {
	
		//	//https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
		//	//// transpose 3x3, we know m03 = m13 = m23 = 0
		//	//__m128 t0 = VecShuffle_0101(inM.mVec[0], inM.mVec[1]); // 00, 01, 10, 11
		//	//__m128 t1 = VecShuffle_2323(inM.mVec[0], inM.mVec[1]); // 02, 03, 12, 13
		//	//r.mVec[0] = VecShuffle(t0, inM.mVec[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
		//	//r.mVec[1] = VecShuffle(t0, inM.mVec[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
		//	//r.mVec[2] = VecShuffle(t1, inM.mVec[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

		//	//													 // last line
		//	//r.mVec[3] = _mm_mul_ps(r.mVec[0], VecSwizzle1(inM.mVec[3], 0));
		//	//r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[1], VecSwizzle1(inM.mVec[3], 1)));
		//	//r.mVec[3] = _mm_add_ps(r.mVec[3], _mm_mul_ps(r.mVec[2], VecSwizzle1(inM.mVec[3], 2)));
		//	//r.mVec[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.mVec[3]);s


		//	//auto t0 = simd::shuffle<0, 1, 0, 1>(_xmm[0], _xmm[1]);
		//	//auto t1 = simd::shuffle<2, 3, 2, 3>(_xmm[0], _xmm[1]);

		//	//auto r0 = simd::shuffle<0, 2, 0, 3>(t0, _xmm[2]);
		//	//auto r1 = simd::shuffle<1, 3, 1, 3>(t0, _xmm[2]);
		//	//auto r2 = simd::shuffle<0, 2, 2, 3>(t1, _xmm[2]);
		//	//
		//	//auto r3 = simd::mul(r0,			 simd::swizzle1<0>(_xmm[3]));
		//	//r3 = simd::add(r3, simd::mul(r1, simd::swizzle1<1>(_xmm[3])));
		//	//r3 = simd::add(r3, simd::mul(r2, simd::swizzle1<2>(_xmm[3])));
		//	//r3 = simd::sub(simd::set(0.0f, 0.0f, 0.0f, 1.0f), r3);

		//	return { r0, r1, r2, r3 };
		//}

		_RDX_INLINE static Mat44 identity() {
			return {
				simd::set(1,0,0,0),
				simd::set(0,1,0,0),
				simd::set(0,0,1,0),
				simd::set(0,0,0,1)
			};
		}

		_RDX_INLINE static Mat44 rotate_y(Scalar alpha) {
			return {
				simd::set(std::cos(alpha),0,-std::sin(alpha),0),
				simd::set(0,1,0,0),
				simd::set(std::sin(alpha),0,std::cos(alpha),0),
				simd::set(0,0,0,1)
			};
		}

		_RDX_INLINE static Mat44 rotate_euler(const vec3_type& angles) {
			auto a = deg2rad(angles.x);
			auto b = deg2rad(angles.y);
			auto c = deg2rad(angles.z);

			return {
				simd::set(
					std::cos(b) * std::cos(c), -std::cos(b) * std::sin(c),
					std::sin(b)
				),
				simd::set(
					std::sin(a) * std::sin(b) * std::cos(c) + std::cos(a) *
					std::sin(c), -std::sin(a) * std::sin(b) * std::sin(c) +
					std::cos(a) * std::cos(c), -std::sin(a) * std::cos(b)
				),
				simd::set(
					-std::cos(a) * std::sin(b) * std::cos(c) + std::sin(a) *
					std::sin(c), std::cos(a) * std::sin(b) * std::sin(c) +
					std::sin(a) * std::cos(c), std::cos(a) * std::cos(b)
				),
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

		_RDX_INLINE static Mat44 perspective(Scalar fov, Scalar aspect, Scalar near, Scalar far) {
			auto yscale = static_cast<Scalar>(-1. / deg2rad(fov / 2.0f));
			auto xscale = static_cast<Scalar>(yscale / aspect);
			auto nf = near - far;

			return {
				simd::set(xscale,0,0,0),
				simd::set(0,yscale,0,0),
				simd::set(0,0,(far + near) / nf, 2 * far * near / nf),
				simd::set(0,0,-1,0)
			};
		}

		_RDX_INLINE static Mat44 translate(const vec3_type& coords) {
			return {
				//TODO: Optimize
				simd::set(1,0,0,coords.x),
				simd::set(0,1,0,coords.y),
				simd::set(0,0,1,coords.z),
				simd::set(0,0,0,1)
			};
		}
		
		_RDX_INLINE static Mat44 lookat(const vec3_type& eye, const vec3_type& center, const vec3_type& up) {

			auto f = (center - eye).normalize();
			auto s = f.cross(up.normalize()).normalize();
			auto u = s.cross(f);

			return {
				simd::set(s.x, s.y, s.z, -s.dot(eye)),
				simd::set(u.x, u.y, u.z, -u.dot(eye)),
				simd::set(-f.x, -f.y, -f.z, f.dot(eye)),
				simd::set(0,0,0,1)
			};
		}

		constexpr vec4_type operator[](std::size_t index) {
			return _xmm[index];
		}

		XMM _xmm[4];
	};

	using Mat44f = Mat44<f32, simd::f32x4>;
}