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
#include "hash.h"
#include "bitset.h"

#include "allocation\default_allocator.h"
#include "allocation\growth_policy.h"

#include <optional> //std::optional

namespace redox {
	namespace detail {
		template<class Key, class Value>
		struct node {
			Key key; Value value;
		};

		template<std::size_t c1, std::size_t c2>
		struct QuadraticProbing {
			std::size_t operator()(std::size_t slot, std::size_t index, std::size_t n) {
				auto cx1 = c1 * index;
				auto cx2 = c2 * index * index;

				return (slot + cx1 + cx2) % n;
			}
		};

		struct LinearProbing {
			std::size_t operator()(std::size_t slot, std::size_t index, std::size_t n) {
				return (slot + index) % n;
			}
		};
	}

	template<class Key, class Value,
	class Allocator = allocation::DefaultAllocator<detail::node<Key, Value>>,
	class GrowthPolicy = allocation::ExponentialGrowth,
	class ProbingPolicy = detail::LinearProbing>
	class Hashmap {
	public:
		using node_type = detail::node<Key, Value>;

		Hashmap() = default;

		_RDX_INLINE Hashmap(std::initializer_list<node_type> values) {
			resize(values.size());
			for (const auto& v : values)
				_push_no_checks(v.key, v.value);
		}

		template<class _Key, class _Value>
		_RDX_INLINE void push(_Key&& key, _Value&& value) {
			_grow_if_needed();
			_push_no_checks(std::forward<_Key>(key), std::forward<_Value>(value));
		}

		_RDX_INLINE const Value* get(const Key& key) const {
			auto slot = _find_key(key);
			if (slot) {
				const auto& entry = _slots[slot.value()];
				return std::addressof(entry.value);
			}
			return nullptr;
		}

		_RDX_INLINE void resize(std::size_t size) {
			_slots.resize(size);
			_slotOccupation.resize(size);
		}

		_RDX_INLINE bool empty() const {
			return _slots.empty();
		}

		_RDX_INLINE void clear() {
			_slots.clear();
			_slotOccupation.clear();
		}

	private:
		template<class _Key, class _Value>
		void _push_no_checks(_Key&& key, _Value&& value) {
			auto slot = _compute_slot(key);
			if (_slotOccupation.get(slot)) {
				if (_slots[slot].key == key)
					throw Exception("only unique keys");

				auto nextSlot = _resolve_collision(slot);
				if (!nextSlot)
					throw Exception("failed to resolve collision");

				slot = nextSlot.value();
			}

			_slotOccupation.set(slot);
			_slots[slot].key = std::forward<_Key>(key);
			_slots[slot].value = std::forward<_Value>(value);
		}

		_RDX_INLINE std::optional<std::size_t> _find_key(const Key& key) const {
			if (empty())
				return std::nullopt;

			auto slot = _compute_slot(key);
			if (_slotOccupation.get(slot)) {
				if (_slots[slot].key == key)
					return slot;

				//Different key resides at computed slot,
				//target key probably collided earlier.
				return _find_collided_key(slot, key);
			}

			return std::nullopt;
		}

		_RDX_INLINE auto _compute_slot(const Key& key) const {
			Hash<Key> fn;
			auto hash = fn(key);
			return hash % _slots.size();
		}

		_RDX_INLINE void _grow_if_needed() {
			if (empty() || _slotOccupation.all()) {
				GrowthPolicy policyFn;
				resize(policyFn(_slots.size()));
			}
		}

		_RDX_INLINE std::optional<std::size_t> _find_collided_key(std::size_t slot, const Key& key) const {
			ProbingPolicy probing;
			for (std::size_t index = 1; index < _slots.size(); ++index) {
				auto nextslot = probing(slot, index, _slots.size());
				if (_slotOccupation.get(nextslot) && _slots[nextslot].key == key)
					return nextslot;
			}
			return std::nullopt;
		}

		_RDX_INLINE std::optional<std::size_t> _resolve_collision(std::size_t slot) const {
			ProbingPolicy probing;
			for (std::size_t index = 1; index < _slots.size(); ++index) {
				auto nextslot = probing(slot, index, _slots.size());
				if (!_slotOccupation.get(nextslot))
					return nextslot;
			}
			return std::nullopt;
		}

		Bitset _slotOccupation;
		Buffer<node_type, Allocator> _slots;
	};
}