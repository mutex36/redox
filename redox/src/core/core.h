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
	typedef float f32;
	typedef double f64;

	typedef char i8;
	typedef int i32;
	typedef long long i64;

	typedef unsigned char u8;
	typedef unsigned int u32;
	typedef unsigned long long u64;

	typedef u8 byte;
}

#include "platform.h"
#include "error.h"

#ifdef RDX_PLATFORM_WINDOWS
	#define _RDX_INLINE __forceinline
	#ifdef RDX_COMPILER_MSC
		#define _RDX_DEBUG_BREAK __debugbreak
		#if _DEBUG
			#define RDX_DEBUG 
		#endif
	#endif
#endif