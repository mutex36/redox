#include "pch.h"

TEST(Buffer, Push) {
	constexpr auto runs = 10000;
	redox::Buffer<int> buffer;
	for (size_t i = 0; i < runs; i++)
		buffer.push(i);
	ASSERT_EQ(buffer.size(), runs);
	for (size_t i = 0; i < runs; i++)
		ASSERT_EQ(buffer[i], i);

	redox::Buffer<int> buffer2(100);
	for (size_t i = 0; i < 100; i++)
		ASSERT_EQ(buffer2[i], 0);

	buffer2.resize(200);
	for (size_t i = 0; i < 200; i++)
		ASSERT_EQ(buffer2[i], 0);
}

TEST(String, Construct) {
	const char test_str[] = "abcdefgh123";
	redox::String str1(test_str);
	ASSERT_STREQ(str1.cstr(), test_str);
	ASSERT_EQ(str1.size(), sizeof(test_str) - 1);

	redox::String str2("");
	ASSERT_STREQ(str2.cstr(), "");
	ASSERT_EQ(str2.size(), 0);
}

TEST(String, CopyMove) {
	redox::String str1("test123");

	redox::String str2(str1);
	ASSERT_EQ(str1, str2);

	redox::String str3;
	str3 = str2;
	ASSERT_EQ(str2, str3);

	redox::String str4(std::move(str3));
	ASSERT_EQ(str4, str2);

	redox::String str5;
	str5 = std::move(str4);
	ASSERT_EQ(str5, str2);

	redox::String empty;
	redox::String empty2;
	empty2 = empty;
	ASSERT_EQ(empty, empty2);
}

TEST(String, Format) {
	auto fmt = redox::format("1234{0}#{1}{0}", 5, "1234");
	ASSERT_STREQ(fmt.cstr(), "12345#12345");

	auto fmt2 = redox::format("{0}", redox::binary((uint8_t)0x80));
	ASSERT_STREQ(fmt2.cstr(), "00000001");
}

TEST(Bitset, SetGet) {

	constexpr auto res = 4194857;
	redox::DynBitset bitset(50);

	bitset.set(0);
	bitset.set(3);
	bitset.set(5);
	bitset.set(9);
	bitset.set(22);

	auto k = bitset.get(9);
	ASSERT_TRUE(k);

	auto k2 = bitset.get(6);
	ASSERT_FALSE(k2);

	auto k3 = bitset.get(22);
	ASSERT_TRUE(k3);

	auto k4 = bitset.get(44);
	ASSERT_FALSE(k4);

	auto j = bitset.to_u64();
	ASSERT_EQ(j, res);


	redox::DynBitset bitset2(12);
	for (size_t i = 0; i < bitset2.size(); i++)
		bitset2.set(i);

	ASSERT_TRUE(bitset2.all());

	bitset2.set(4, false);

	ASSERT_FALSE(bitset2.all());
}

TEST(Hashmap, SetGet) {
	redox::Hashmap<size_t, size_t> hm;

	for (size_t i = 0; i < 1000; i++)
		hm.push(i, i);

	for (size_t i = 0; i < 1000; i++)
		ASSERT_EQ(hm.get(i).value(), i);

}

struct Foo {
	Foo(int _a, int _b) : a(_a), b(_b) {
	}
	int a, b;
};

TEST(SmartPtr, Construct) {
	auto original = redox::make_smart_ptr<Foo>(12, 44);

	ASSERT_EQ(original->a, 12);
	ASSERT_EQ(original->b, 44);

	auto copy = std::move(original);

	ASSERT_EQ(copy->a, 12);
	ASSERT_EQ(copy->b, 44);
}

TEST(RefCounted, Construct) {
	auto original = redox::make_ref_counted<Foo>(12, 44);

	auto copy1 = original;
	ASSERT_EQ(copy1.ref_count(), 2);

	auto copy2(std::move(copy1));
	ASSERT_EQ(copy2.ref_count(), 2);

	auto copy3 = copy2;
	ASSERT_EQ(copy3.ref_count(), 3);

	redox::RefCounted<Foo> empty;
	empty = copy3;
	ASSERT_EQ(copy3.ref_count(), 4);

	ASSERT_EQ(copy3->a, 12);
	ASSERT_EQ(copy3->b, 44);
}


TEST(Vec, Ops) {

	redox::Vec3f a(3.0f, 3.0f, 3.0f);
	redox::Vec3f b(3.0f, 3.0f, 3.0f);

	auto c = a + b;
	ASSERT_FLOAT_EQ(c.x, 6.0f);
	ASSERT_FLOAT_EQ(c.y, 6.0f);
	ASSERT_FLOAT_EQ(c.x, 6.0f);

	auto d = a * b;
	ASSERT_FLOAT_EQ(d.x, 9.0f);
	ASSERT_FLOAT_EQ(d.y, 9.0f);
	ASSERT_FLOAT_EQ(d.x, 9.0f);

	auto e = d * 4.0f;
	ASSERT_FLOAT_EQ(e.x, 36.0f);
	ASSERT_FLOAT_EQ(e.y, 36.0f);
	ASSERT_FLOAT_EQ(e.x, 36.0f);

	auto le = e.Length();
	ASSERT_FLOAT_EQ(le, 62.353829072479584);

	auto adb = a.Dot(b);
	ASSERT_FLOAT_EQ(adb, 27.0f);
}