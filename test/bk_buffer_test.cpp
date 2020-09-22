#include "gtest/gtest.h"
#include "bk_buffer.h"

static void test_dtor(void* ptr)
{
	*(bool*)ptr = true;
}

TEST(bk_buffer, bk_buffer_create_with_data)
{
	bool* tmp = new bool(false);
	bk_buffer* pBuffer = bk_buffer_create_with_data(tmp, 1, test_dtor);
	bk_buffer_destroy(pBuffer);
	ASSERT_TRUE(*tmp);
	delete tmp;
}

TEST(bk_buffer, reserve)
{
	bk_buffer* pBuffer = bk_buffer_create(32);
	bk_buffer_reserve(pBuffer, 16);
	ASSERT_EQ(bk_buffer_get_capacity(pBuffer), 32);
	bk_buffer_reserve(pBuffer, 64);
	ASSERT_EQ(bk_buffer_get_capacity(pBuffer), 64);
	bk_buffer_destroy(pBuffer);

	char buf[32];
	pBuffer = bk_buffer_create_with_data(buf, 32, BK_BUFFER_STATIC);
	bk_buffer_reserve(pBuffer, 16);
	ASSERT_EQ(bk_buffer_get_capacity(pBuffer), 32);
	bk_buffer_reserve(pBuffer, 64);
	ASSERT_EQ(bk_buffer_get_capacity(pBuffer), 64);
	bk_buffer_destroy(pBuffer);
}

TEST(bk_buffer, read)
{
	char str[11] = "0123546789";
	char buf[64] = {};
	bk_buffer* pBuffer = bk_buffer_create_with_data(str, sizeof(str), BK_BUFFER_STATIC);
	bk_buffer_limit(pBuffer, sizeof(str));
	bk_buffer_read(pBuffer, buf, sizeof(buf));
	bk_buffer_destroy(pBuffer);
	ASSERT_STREQ(buf, str);
}

TEST(bk_buffer, write)
{
	char str[11] = "0123546789";
	char buf[64] = {};
	bk_buffer* pBuffer = bk_buffer_create_with_data(buf, sizeof(buf), BK_BUFFER_STATIC);
	bk_buffer_limit(pBuffer, sizeof(str));
	bk_buffer_write(pBuffer, str, sizeof(str));
	bk_buffer_destroy(pBuffer);
	ASSERT_STREQ(buf, str);
}

TEST(bk_buffer, flip)
{
	char buf2[64] = {};
	char buf[64] = {};
	bk_buffer* pBuffer = bk_buffer_create_with_data(buf, sizeof(buf), BK_BUFFER_STATIC);
	bk_buffer_write(pBuffer, "0123456789", 10);
	bk_buffer_flip(pBuffer);
	bk_buffer_read(pBuffer, buf2, sizeof(buf2));
	bk_buffer_destroy(pBuffer);
	ASSERT_STREQ(buf, buf2);
}


TEST(bk_buffer, mark_reset)
{
	char buf2[64] = {};
	char buf[64] = {};
	bk_buffer* pBuffer = bk_buffer_create_with_data(buf, sizeof(buf), BK_BUFFER_STATIC);
	bk_buffer_write(pBuffer, "012345", 6);
	bk_buffer_mark(pBuffer);
	bk_buffer_write(pBuffer, "6789", 4);
	bk_buffer_reset(pBuffer);
	bk_buffer_write(pBuffer, "abc", 3);
	bk_buffer_seek(pBuffer, 1, SEEK_CUR);
	bk_buffer_flip(pBuffer);
	bk_buffer_read(pBuffer, buf2, sizeof(buf2));
	bk_buffer_destroy(pBuffer);
	ASSERT_STREQ(buf2, "012345abc9");
}