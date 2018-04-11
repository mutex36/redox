#include "pch.h"

TEST(Buffer, Push) {
	constexpr auto runs = 10000;
	redox::Buffer<int> buffer;
	for (size_t i = 0; i < runs; i++)
		buffer.push(i);
	ASSERT_EQ(buffer.size(), runs);
	for (size_t i = 0; i < runs; i++)
		ASSERT_EQ(buffer[i], i);
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
}
