#include "pch.h"


TEST(Vec, Ops) {
	redox::math::Vec3f a(3.0f, 3.0f, 3.0f);
	redox::math::Vec3f b(3.0f, 3.0f, 3.0f);

	auto c = a + b;
	ASSERT_FLOAT_EQ(c.x, 6.0f);
	ASSERT_FLOAT_EQ(c.y, 6.0f);
	ASSERT_FLOAT_EQ(c.z, 6.0f);

	auto d = a * b;
	ASSERT_FLOAT_EQ(d.x, 9.0f);
	ASSERT_FLOAT_EQ(d.y, 9.0f);
	ASSERT_FLOAT_EQ(d.z, 9.0f);

	auto e = d * 4.0f;
	ASSERT_FLOAT_EQ(e.x, 36.0f);
	ASSERT_FLOAT_EQ(e.y, 36.0f);
	ASSERT_FLOAT_EQ(e.z, 36.0f);

	auto le = e.length();
	ASSERT_FLOAT_EQ(le, 62.353829072479584f);

	auto adb = a.dot(b);
	ASSERT_FLOAT_EQ(adb, 27.0f);

	redox::math::Vec3f ax(1,2,3);
	redox::math::Vec3f ay(1,5,7);
	auto crs = ax.cross(ay);

	ASSERT_FLOAT_EQ(crs.x, -1.0f);
	ASSERT_FLOAT_EQ(crs.y, -4.0f);
	ASSERT_FLOAT_EQ(crs.z, 3.0f);

	redox::math::Vec4f kkh;
	kkh.x = 1;
	kkh.y = 2;
	kkh.z = 3;
	kkh.w = 4;

	ASSERT_FLOAT_EQ(kkh.x, 1.0f);
	ASSERT_FLOAT_EQ(kkh.y, 2.0f);
	ASSERT_FLOAT_EQ(kkh.z, 3.0f);
	ASSERT_FLOAT_EQ(kkh.w, 4.0f);

}

TEST(Mat, Ops) {

	auto scm = redox::math::Mat44f::scale({3,3,3});
	ASSERT_FLOAT_EQ(scm[0].x, 3);
	ASSERT_FLOAT_EQ(scm[1].y, 3);
	ASSERT_FLOAT_EQ(scm[2].z, 3);
	ASSERT_FLOAT_EQ(scm[3].w, 1);

	auto identity = redox::math::Mat44f::identity();
	ASSERT_FLOAT_EQ(identity[0].x, 1);
	ASSERT_FLOAT_EQ(identity[1].y, 1);
	ASSERT_FLOAT_EQ(identity[2].z, 1);
	ASSERT_FLOAT_EQ(identity[3].w, 1);

	auto sum = scm + identity;
	ASSERT_FLOAT_EQ(sum[0].x, 4);
	ASSERT_FLOAT_EQ(sum[1].y, 4);
	ASSERT_FLOAT_EQ(sum[2].z, 4);
	ASSERT_FLOAT_EQ(sum[3].w, 2);

	redox::math::Mat44f ima({
		1,0,0,1,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	});
	
	//auto ivma = ima.inverse();

}