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
#include "core\ref_counted.h"
#include "core\string.h"

static const redox::String
	RDX_RESOURCE_PATH(R"(C:\Users\luis9\Desktop\redox\redox\resources\)");

static const redox::String
	RDX_CONFIG_PATH(R"(C:\Users\luis9\Desktop\redox\redox\config\)");

#define RDX_ASSET(file) RDX_RESOURCE_PATH + file
#define RDX_FIND_ASSET(folder, file) RDX_RESOURCE_PATH + folder + file
#define RDX_CONFIG_ASSET(file) RDX_CONFIG_PATH + file

namespace redox {
	
	template<class T>
	using Resource = RefCounted<T>;

}