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
#include "buffer.h"
#include "dynamic_bitset.h"
#include "allocation\default_allocator.h"
#include "allocation\growth_policy.h"

#include <optional> //std::optional

namespace redox {

	template<class Key, class Value>
	struct Pair {
		Key key;
		Value value;
	};

	template<class T>
	struct Hash {
		std::size_t operator()(const T& exp) {
			return static_cast<std::size_t>(exp);
		}
	};

	struct LinearProbing {
		std::size_t operator()(std::size_t slot, std::size_t index, std::size_t n) {
			return (slot + index) % n;
		}
	};
	
	template<class Key, class Value,
		class Allocator = allocation::DefaultAllocator<Pair<Key, Value>>,
		class GrowthPolicy = allocation::ExponentialGrowth,
		class ProbingPolicy = LinearProbing>
		class Hashmap {
		public:
			Hashmap(std::size_t size) : _data(size), _flags(size) {
			}
			Hashmap() = default;
			~Hashmap() = default;

			template<class _Key, class _Value>
			void push(_Key&& key, _Value&& value) {
				_check_realloc();

				auto slot = _hash(key);
				if (_flags.get(slot)) {
					if (_data[slot].key == key)
						throw Exception("only unique keys");
					slot = _resolve_collision(slot);
				}

				_flags.set(slot);
				_data[slot].key = std::forward<_Key>(key);
				_data[slot].value = std::forward<_Value>(value);
			}

			std::optional<Value> get(const Key& key) const {
				if (empty())
					throw Exception("hashmap is empty");

				auto slot = _hash(key);
				if (_flags.get(slot)) {
					if (_data[slot].key == key)
						return _data[slot].value;
					auto result = _find_collided_key(slot, key);
					if (result) 
						return _data[result.value()].value;
				}
				return std::nullopt;
			}

			_RDX_INLINE bool empty() const {
				return _data.size() == 0;
			}

		private:
			_RDX_INLINE bool _full() const {
				return _flags.all();
			}

			_RDX_INLINE std::size_t _hash(const Key& key) const {
				Hash<Key> fn;
				std::size_t hash = fn(key);
				return hash % _data.size();
			}

			std::optional<std::size_t> _find_collided_key(std::size_t slot, const Key& key) const {
				ProbingPolicy probing;
				for (std::size_t index = 1; index < _data.size(); ++index) {
					auto nextslot = probing(slot, index, _data.size());
					if (_flags.get(nextslot) && _data[nextslot].key == key)
						return nextslot;
				}
				return std::nullopt;
			}

			std::size_t _resolve_collision(std::size_t slot) const {
				ProbingPolicy probing;
				for (std::size_t index = 1; index < _data.size(); ++index) {
					auto nextslot = probing(slot, index, _data.size());
					if (!_flags.get(nextslot))
						return nextslot;
				}
				throw Exception("failed to resolve collision");
			}

			void _check_realloc() {
				if (empty() || _full()) {
					GrowthPolicy fn;
					auto grow = fn(_data.size());
					_data.resize(grow);
					_flags.resize(grow);
				}
			}

			DynBitset _flags;
			Buffer<Pair<Key, Value>, Allocator> _data;
	};
}