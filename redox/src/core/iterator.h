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