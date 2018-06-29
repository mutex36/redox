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
			template<class _Key, class...ValueArgs>
			node(_Key&& key, ValueArgs&&...args) :
				key(std::forward<_Key>(key)),
				value(std::forward<ValueArgs>(args)...) {
			}

			Key key;
			Value value;
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
	class Hashmap : public NonCopyable {
	public:
		using node_type = detail::node<Key, Value>;
		using size_type = std::size_t;

		template<class _Key,
			class = std::enable_if_t<std::is_constructible_v<Key, std::decay_t<_Key>>>>
		Hashmap(_Key&& nullKey) :
			_nullKey(std::forward<_Key>(nullKey)),
			_data(nullptr), _size(0), _slots(0) {
		}

		template<class _Key,
			class = std::enable_if_t<std::is_constructible_v<Key, std::decay_t<_Key>>>>
		_RDX_INLINE Hashmap(_Key&& nullKey, std::initializer_list<node_type> values) : 
			_nullKey(std::forward<_Key>(nullKey)) {
			resize(values.size());
			for (const auto& v : values)
				_push_no_checks(v.key, v.value);
		}

		_RDX_INLINE ~Hashmap() {
			_destruct();
			_dealloc();
		}

		_RDX_INLINE Hashmap(Hashmap&& ref) :
			_nullKey(std::move(ref._nullKey)),
			_data(ref._data),
			_size(ref._size),
			_slots(ref._slots) {
		
			ref._data = nullptr;
			ref._size = 0;
			ref._slots = 0;
		}

		_RDX_INLINE Hashmap& operator=(Hashmap&& ref) {
			_nullKey = std::move(ref._nullKey);
			_data = ref._data;
			_size = ref._size;
			_slots = ref._slots;

			ref._data = nullptr;
			ref._size = 0;
			ref._slots = 0;
			return *this;
		}
		template<class _Key, class _Value>
		_RDX_INLINE void push(_Key&& key, _Value&& value) {
			_grow_if_needed();
			_push_no_checks(std::forward<_Key>(key), std::forward<_Value>(value));
		}

		template<class _Key, class...Args>
		_RDX_INLINE Value& emplace(_Key&& key, Args&&...args) {
			_grow_if_needed();
			return _emplace_no_checks(std::forward<_Key>(key), std::forward<Args>(args)...);
		}

		_RDX_INLINE bool has_key(const Key& key) const {
			return _find_key(key).has_value();
		}

		_RDX_INLINE const Value* get(const Key& key) const {
			auto slot = _find_key(key);
			if (slot)
				//TODO: std::optional<&>
				return std::addressof(_data[slot.value()].value);
			return nullptr;
		}

		_RDX_INLINE void resize(std::size_t slots) {
			if (slots <= _slots) return;
			auto oldData = _data;
			auto oldSlots = _slots;
			RDX_SCOPE_GUARD([oldData]() {
				Allocator::deallocate(oldData);
			});

			_data = Allocator::allocate(slots);
			_slots = slots;
			_size = 0;
			_init_slots();

			//The slot size changed, existing items need to be rehashed
			for (std::size_t slot = 0; slot < oldSlots; slot++) {
				auto& node = oldData[slot];
				if (node.key != _nullKey)
					_push_no_checks(std::move(node.key), std::move(node.value));
				node.~node_type();
			}
		}

		_RDX_INLINE void clear() {
			_destruct();
			_init_slots();
		}

		_RDX_INLINE bool empty() const {
			return _size == 0;
		}

	private:
		_RDX_INLINE void _init_slots() {
			for (std::size_t slot = 0; slot < _slots; slot++)
				new (_data + slot) node_type(_nullKey);
		}

		_RDX_INLINE void _destruct() {
			_size = 0;
			for (std::size_t slot = 0; slot < _slots; slot++)
				_data[slot].~node_type();
		}

		_RDX_INLINE void _dealloc() {
			Allocator::deallocate(_data);
		}

		_RDX_INLINE bool _occupied(std::size_t slot) const {
			return _data[slot].key != _nullKey;
		}

		template<class _Key, class...Args>
		_RDX_INLINE Value& _emplace_no_checks(_Key&& key, Args&&...args) {
			auto slot = _find_empty_slot(key);
			auto node = new (_data + slot) node_type(
				std::forward<_Key>(key), std::forward<Args>(args)...);
			_size++;
			return node->value;
		}

		template<class _Key, class _Value>
		_RDX_INLINE void _push_no_checks(_Key&& key, _Value&& value) {
			auto slot = _find_empty_slot(key);
			new (_data + slot) node_type(
				std::forward<_Key>(key), std::forward<_Value>(value));
			_size++;
		}

		_RDX_INLINE std::size_t _find_empty_slot(const Key& key) {
			auto slot = _compute_slot(key);
			if (_occupied(slot)) {
				if (_data[slot].key == key)
					throw Exception("only unique keys");

				auto nextSlot = _resolve_collision(slot);
				if (!nextSlot)
					throw Exception("failed to resolve collision");

				slot = nextSlot.value();
			}
			return slot;
		}

		_RDX_INLINE std::optional<std::size_t> _find_key(const Key& key) const {
			if (empty())
				return std::nullopt;

			auto slot = _compute_slot(key);
			if (_occupied(slot)) {
				if (_data[slot].key == key)
					return slot;

				//Different key resides at computed slot,
				//target key probably collided earlier.
				return _find_collided_key(slot, key);
			}
			return std::nullopt;
		}

		_RDX_INLINE auto _compute_slot(const Key& key) const {
			Hash<Key> fn;
			return fn(key) % _slots;
		}

		_RDX_INLINE void _grow_if_needed() {
			if (_size == _slots) {
				GrowthPolicy policyFn;
				resize(policyFn(_size));
			}
		}

		_RDX_INLINE std::optional<std::size_t> _find_collided_key(std::size_t slot, const Key& key) const {
			ProbingPolicy probing;
			for (std::size_t index = 1; index < _slots; ++index) {
				auto nextslot = probing(slot, index, _slots);
				if (_occupied(nextslot) && _data[nextslot].key == key)
					return nextslot;
			}
			return std::nullopt;
		}

		_RDX_INLINE std::optional<std::size_t> _resolve_collision(std::size_t slot) const {
			ProbingPolicy probing;
			for (std::size_t index = 1; index < _slots; ++index) {
				auto nextslot = probing(slot, index, _slots);
				if (!_occupied(nextslot))
					return nextslot;
			}
			return std::nullopt;
		}

		node_type* _data;
		size_type _slots;
		size_type _size;
		Key _nullKey;
	};
}