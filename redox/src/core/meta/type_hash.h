#pragma once

#define RDX_TYPE_HASH(Type)									\
template<>													\
struct redox::reflection::type_to_hash<Type> {				\
	static constexpr size_t hash = __COUNTER__;				\
};															\
template<>													\
struct redox::reflection::hash_to_type<						\
	redox::reflection::type_to_hash<Type>::hash> {			\
	using type = Type;										\
};															\

namespace redox::reflection {
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