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
#include "core\core.h"

#ifdef RDX_PLATFORM_WINDOWS
#include "timer.h"
#include "windows.h"

struct redox::platform::Timer::internal {
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER freq;
};

redox::platform::Timer::Timer() {
	_internal = make_smart_ptr<internal>();
	QueryPerformanceFrequency(&_internal->freq);
}

redox::platform::Timer::~Timer() {
}

void redox::platform::Timer::start() {
	QueryPerformanceCounter(&_internal->start);
}

void redox::platform::Timer::reset() {
	QueryPerformanceCounter(&_internal->start);
}

redox::f64 redox::platform::Timer::elapsed() {
	QueryPerformanceCounter(&_internal->end);
	return static_cast<redox::f64>(
		_internal->end.QuadPart - _internal->start.QuadPart) / _internal->freq.QuadPart * 1000.;
}

redox::f64 redox::platform::Timer::freq() {
	return static_cast<redox::f64>(_internal->freq.QuadPart);
}
#endif