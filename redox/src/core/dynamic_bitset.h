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
#include "core\buffer.h"
#include "core\non_copyable.h"

#include "math\util.h"
#include "logging\log.h"

#include <limits> //std::numeric_limits

namespace redox {
	namespace detail {
		template<class UnitType, 
			class Allocator = allocation::DefaultAllocator<UnitType>>
		class DynBitset : public NonCopyable {
		public:
			static constexpr auto bits_per_unit = sizeof(UnitType) * 8;
			static constexpr auto unit_0ff_mask 
				= std::numeric_limits<UnitType>::max();

			DynBitset() : _size(0) {
			}
			DynBitset(std::size_t size) {
				resize(size);
			}
			~DynBitset() = default;

			DynBitset(DynBitset&& ref) : _size(ref._size), _units(std::move(ref._units)) {
			}

			_RDX_INLINE DynBitset& operator=(DynBitset&& ref) {
				_size = ref._size;
				_units = std::move(ref._units);
				return *this;
			}

			_RDX_INLINE void resize(std::size_t size) {
				_units.resize(_required_units(size));
				_size = size;
			}

			void set(std::size_t index, bool value = true) {
				auto units = _required_units(index + 1);
				if (units > _units.size())
					throw Exception("bit not found");

				auto bitindex = index % bits_per_unit;
				auto mask = static_cast<UnitType>(0x1) << bitindex;
				auto& unit = _units[units - 1];

				//https://graphics.stanford.edu/~seander/bithacks.html
				unit ^= (-static_cast<UnitType>(value) ^ unit) & mask;
			}

			u64 to_u64() const {
				if (_units.size() > sizeof(u64) / sizeof(UnitType))
					throw Exception("u64 overflow");

				u64 output = 0; u64 index = 0;
				for (auto& unit : _units) {
					auto unit64 = static_cast<u64>(unit);
					output |= (unit64 << (index++ * bits_per_unit));
				}
				return output;
			}

			_RDX_INLINE std::size_t max_bits() const {
				return _units.size() * bits_per_unit;
			}
			
			_RDX_INLINE std::size_t size() const {
				return _size;
			}

			bool all() const {
				if (_size == 0) return false;

				std::size_t index = 0;
				while(index < _units.size() - 1) {
					if ((_units[index++] & unit_0ff_mask) != unit_0ff_mask)
						return false;
				}

				auto activebits = _size % bits_per_unit;
				auto mask = (static_cast<UnitType>(0x1) << activebits) - 1;
				return (_units[index] & mask) == mask;
			}

			bool get(std::size_t index) const {
				auto units = _required_units(index + 1);
				if (units > _units.size())
					throw Exception("bit not found");

				auto bitindex = index % bits_per_unit;
				auto mask = static_cast<UnitType>(0x1) << bitindex;
				return static_cast<bool>(_units[units - 1] & mask);
			}

		private:
			_RDX_INLINE auto _required_units(std::size_t size) const {
				auto reqbits = math::round_multiple(size, bits_per_unit);
				return reqbits / bits_per_unit;
			}

			std::size_t _size;
			Buffer<UnitType, Allocator> _units;
		};
	}

	using DynBitset = detail::DynBitset<u8>;
}