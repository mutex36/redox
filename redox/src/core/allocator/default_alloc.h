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

namespace redox::allocator {
	/**
	* DefaultAllocator
	* A malloc/free based allocator
	*/
	template<typename T>
	struct DefaultAllocator {

		/**
		* DefaultAllocator::allocate(n)
		* Allocate a chunk of memory
		*/
		static T* allocate(const std::size_t n = 1) {
			return reinterpret_cast<T*>(
				std::malloc(sizeof(T) * n));
		}

		/**
		* DefaultAllocator::deallocate(p)
		* Deallocate previously allocated memory
		*/
		static void deallocate(T* ptr) {
			if (ptr != nullptr)
				std::free(ptr);
		}
	};
}