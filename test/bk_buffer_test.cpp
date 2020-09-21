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

TEST(bk_buffer, read_write)
{

}