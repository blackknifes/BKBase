#include "bk_data.h"
#include "gtest/gtest.h"

TEST(bk_data, all)
{
	bk_data* data = bk_data_create(1024);
	bk_data* subData = bk_data_create_with_sub(data, 10, 24);

	ASSERT_EQ(bk_data_get_size(data), 1024);
	ASSERT_EQ(bk_data_get_size(subData), 24);

	bk_data_write(data, "012345678901234567890123\0", 0, 25);
	const char* str1 = (const char*)bk_data_get_data(data);
	const char* str2 = (const char*)bk_data_get_data(subData);

	ASSERT_STRCASEEQ(str1, "012345678901234567890123");
	ASSERT_STRCASEEQ(str2, "01234567890123");

	bk_data_write(subData, "1234\0", 0, 5);
	ASSERT_STRCASEEQ(str1, "01234567891234");
	ASSERT_STRCASEEQ(str2, "1234");
	bk_data_destroy(subData);
	bk_data_destroy(data);
}