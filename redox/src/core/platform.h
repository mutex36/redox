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

#if defined _WIN32 || defined _WIN64
#define RDX_PLATFORM_WINDOWS
#elif defined __unix__ || defined unix
#define RDX_PLATFORM_UNIX
#elif defined __linux__
#define RDX_PLATFORM_LINUX
#elif defined __APPLE__ || defined __MACH__
#define RDX_PLATFORM_OSX
#endif

#if defined _MSC_VER
#define RDX_COMPILER_MSC

#if _DEBUG
#define RDX_DEBUG 
#endif
#elif defined __GNUC__ || defined __GNUG__
#define RDX_COMPILER_GCC
#elif defined __clang__
#define RDX_COMPILER_CLANG
#endif