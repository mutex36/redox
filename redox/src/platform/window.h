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
#include "core\core.h"
#include "core\utility.h"

#include "filesystem.h"

namespace redox::platform {
	class Window {
	public:
		enum class Event {
			CLOSE, MINIMIZE, LOSTFOCUS, GAINFOCUS
		};

		using EventFn = Function<void(Event)>;

		Window(const String& name);
		~Window();

		void show() const;
		void process_events() const;
		void hide() const;
		void set_title(const String& title);
		void set_callback(EventFn&& fn);
		bool is_minimized() const;
		void* native_handle() const;

		//internal
		void _notify_event(const Event ev);

	private:
		EventFn _eventfn;

		struct internal;
		UniquePtr<internal> _internal;
	};
}