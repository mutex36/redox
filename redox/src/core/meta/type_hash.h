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

#define RDX_TYPE_HASH(Type)									\
template<>													\
struct redox::reflection::type_to_hash<Type> {				\
	static constexpr hash_type hash = __COUNTER__;			\
};															\
template<>													\
struct redox::reflection::hash_to_type<						\
	redox::reflection::type_to_hash<Type>::hash> {			\
	using type = Type;										\
};															\

namespace redox::reflection {
	using hash_type = std::size_t;

	template<typename T>
	struct type_to_hash;

	template<size_t H>
	struct hash_to_type;
}

RDX_TYPE_HASH(char);
RDX_TYPE_HASH(unsigned char);
RDX_TYPE_HASH(short);
RDX_TYPE_HASH(unsigned short);
RDX_TYPE_HASH(int);
RDX_TYPE_HASH(unsigned int);
RDX_TYPE_HASH(long long);
RDX_TYPE_HASH(unsigned long long);
RDX_TYPE_HASH(float);
RDX_TYPE_HASH(double);