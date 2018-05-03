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

#include "simd.h"

namespace redox::math {
	namespace detail {
		//Accessing scalar by union seems portable, but in C++ UB and (potentially) inefficient.
		//https://stackoverflow.com/questions/12624466/get-member-of-m128-by-index
		template<class Scalar, class XMM, std::size_t Size>
		struct VecBase;

		template<class Scalar, class XMM>
		struct VecBase<Scalar, XMM, 2> {
			VecBase(XMM xmm) : data(xmm) {}
			VecBase(Scalar x, Scalar y)
				: VecBase(simd::set(x,y)) {
			}
			union alignas(simd::alignment) {
				Scalar x, y;
				XMM data;
			};
		};

		template<typename Scalar, typename XMM>
		struct VecBase<Scalar, XMM, 3> {
			VecBase(XMM xmm) : data(xmm) {}
			VecBase(Scalar x, Scalar y, Scalar z)
				: VecBase(simd::set(x, y, z)) {
			}

			_RDX_INLINE VecBase Cross(const VecBase& rhs) const {
				return simd::sub(
					simd::mul(
						simd::shuffle<3, 0, 2, 1>(data, data),
						simd::shuffle<3, 1, 0, 2>(rhs.data, rhs.data)),
					simd::mul(
						simd::shuffle<3, 1, 0, 2>(data, data),
						simd::shuffle<3, 0, 2, 1>(rhs.data, rhs.data))
				);
			}

			union alignas(simd::alignment) {
				Scalar x, y, z;
				XMM data;
			};
		};

		template<class Scalar, class XMM>
		struct VecBase<Scalar, XMM, 4> {
			VecBase(XMM xmm) : data(xmm) {}
			VecBase(Scalar x, Scalar y, Scalar z, Scalar w) 
				: VecBase(simd::set(x, y, z, w)) {
			}
			union alignas(simd::alignment) {
				Scalar x, y, z, w;
				XMM data;
			};
		};
	}
	
	template<class Scalar, class XMM, std::size_t Size>
	struct Vec : detail::VecBase<Scalar, XMM, Size> {
		using base_type = detail::VecBase<Scalar, XMM, Size>;
		using base_type::base_type;

		Vec() : base_type(simd::set_zero()) {}

		_RDX_INLINE Vec operator+(const Vec& rhs) const {
			return simd::add(data, rhs.data);
		}
		_RDX_INLINE Vec operator-(const Vec& rhs) const {
			return simd::sub(data, rhs.data);
		}
		_RDX_INLINE Vec operator*(const Vec& rhs) const {
			return simd::mul(data, rhs.data);
		}
		_RDX_INLINE Vec operator/(const Vec& rhs) const {
			return simd::div(data, rhs.data);
		}

		_RDX_INLINE Vec operator+(Scalar rhs) const {
			return simd::add(data, simd::set_all(rhs));
		}
		_RDX_INLINE Vec operator-(Scalar rhs) const {
			return simd::sub(data, simd::set_all(rhs));
		}
		_RDX_INLINE Vec operator*(Scalar rhs) const {
			return simd::mul(data, simd::set_all(rhs));
		}
		_RDX_INLINE Vec operator/(Scalar rhs) const {
			return simd::div(data, simd::set_all(rhs));
		}

		_RDX_INLINE Scalar Dot(const Vec& rhs) const {
			return simd::extract_lower(
				simd::dot<0x71>(data, rhs.data));
		}

		_RDX_INLINE Scalar Length() const {
			return simd::extract_lower(
				simd::sqrt_lower(simd::dot<0x71>(data, data))
			);
		}
		_RDX_INLINE Vec Normalize() const {
			return simd::mul(data, 
				simd::rsqrt(simd::dot<0x77>(data, data)));
		}
	};

	using Vec4f = Vec<f32, simd::f32x4, 4>;
	using Vec3f = Vec<f32, simd::f32x4, 3>;
	using Vec2f = Vec<f32, simd::f32x4, 2>;
}