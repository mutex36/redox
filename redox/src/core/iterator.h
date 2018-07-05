#pragma once

namespace redox {

	template<class ElementType, class Derived>
	class Iterator {
	public:
		Iterator(ElementType* ptr) : _ptr(ptr) {}

		Iterator& operator++() {
			static_cast<Derived*>(this)->increment();
			return *this;
		}

		Iterator& operator--() {
			static_cast<Derived*>(this)->decrement();
			return *this;
		}

		bool operator!=(const Iterator& rhs) {
			return _ptr != rhs._ptr;
		}

		bool operator==(const Iterator& rhs) {
			return _ptr == rhs._ptr;
		}

		operator bool() const {
			return _ptr != nullptr;
		}

		ElementType& operator*() {
			return *_ptr;
		}

		const ElementType& operator*() const {
			return *_ptr;
		}

		ElementType* operator->() {
			return _ptr;
		}

		const ElementType* operator->() const {
			return _ptr;
		}

	protected:
		ElementType * _ptr;
	};
}