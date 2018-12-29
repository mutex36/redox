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
#include "util.h"
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

		RDX_INLINE Mat44 operator+(const Mat44& rhs) const {
			return {
				simd::add(_xmm[0], rhs._xmm[0]),
				simd::add(_xmm[1], rhs._xmm[1]),
				simd::add(_xmm[2], rhs._xmm[2]),
				simd::add(_xmm[3], rhs._xmm[3])
			};
		}

		RDX_INLINE static Mat44 identity() {
			return {
				simd::set(1,0,0,0),
				simd::set(0,1,0,0),
				simd::set(0,0,1,0),
				simd::set(0,0,0,1)
			};
		}

		RDX_INLINE static Mat44 rotate_y(Scalar alpha) {
			return {
				simd::set(std::cos(alpha),0,-std::sin(alpha),0),
				simd::set(0,1,0,0),
				simd::set(std::sin(alpha),0,std::cos(alpha),0),
				simd::set(0,0,0,1)
			};
		}

		RDX_INLINE static Mat44 rotate_euler(const vec3_type& angles) {
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

		RDX_INLINE static Mat44 scale(const vec3_type& scale) {
			return {
				simd::blend<0x1>(simd::set_zero(), scale._xmm),
				simd::blend<0x2>(simd::set_zero(), scale._xmm),
				simd::blend<0x4>(simd::set_zero(), scale._xmm),
				simd::set(0,0,0,1)
			};
		}

		RDX_INLINE static Mat44 perspective(Scalar fov, Scalar aspect, Scalar near, Scalar far) {
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

		RDX_INLINE static Mat44 translate(const vec3_type& coords) {
			return {
				//TODO: Optimize
				simd::set(1,0,0,coords.x),
				simd::set(0,1,0,coords.y),
				simd::set(0,0,1,coords.z),
				simd::set(0,0,0,1)
			};
		}
		
		RDX_INLINE static Mat44 lookat(const vec3_type& eye, const vec3_type& center, const vec3_type& up) {
			//TODO: Optimize
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

		RDX_INLINE vec4_type operator[](std::size_t index) {
			return _xmm[index];
		}

		XMM _xmm[4];
	};

	using Mat44f = Mat44<f32, simd::f32x4>;
}