#include "gtest/gtest.h"
#include "bk_list.h"

TEST(bk_list, getter)
{
	bk_list* list = bk_list_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_list_push_back(list, &i);
	int* ptr = (int*)bk_list_begin(list);
	for (int i = 0; ptr != bk_list_end(list); ++i, ptr = (int*)bk_list_next(list, ptr))
		ASSERT_EQ(*ptr, i);
	ASSERT_EQ(*(int*)bk_list_get_front(list), 0);
	ASSERT_EQ(*(int*)bk_list_get_back(list), 127);
	ASSERT_EQ(bk_list_get_size(list), 128);

	bk_list_destroy(list);
}

TEST(bk_list, insert)
{
	bk_list* list = bk_list_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
	{
		if (i == 100)
			continue;
		bk_list_push_back(list, &i);
	}
	int* ptr = (int*)bk_list_begin(list);
	for (int i = 0; ptr != bk_list_end(list); ++i, ptr = (int*)bk_list_next(list, ptr))
	{
		if (i < 100)
			ASSERT_EQ(*ptr, i);
		else if (i >= 100)
			ASSERT_EQ(*ptr, i + 1);
	}

	ptr = (int*)bk_list_begin(list);

	for (size_t i = 0; i < 100; ++i)
		ptr = (int*)bk_list_next(list, ptr);

	ASSERT_EQ(bk_list_get_size(list), 127);
	int val = 100;
	bk_list_insert(list, &val, ptr);
	ASSERT_EQ(bk_list_get_size(list), 128);

	ptr = (int*)bk_list_begin(list);
	for (int i = 0; ptr != bk_list_end(list); ++i, ptr = (int*)bk_list_next(list, ptr))
		ASSERT_EQ(*ptr, i);
	bk_list_destroy(list);
}

TEST(bk_list, erase)
{
	bk_list* list = bk_list_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_list_push_back(list, &i);

	int* ptr = (int*)bk_list_begin(list);
	for (int i = 0; ptr != bk_list_end(list); ++i, ptr = (int*)bk_list_next(list, ptr))
		ASSERT_EQ(*ptr, i);

	ptr = (int*)bk_list_begin(list);
	for (size_t i = 0; i < 100; ++i)
		ptr = (int*)bk_list_next(list, ptr);
	ptr = (int*)bk_list_erase(list, ptr);

	ptr = (int*)bk_list_begin(list);
	for (int i = 0; ptr != bk_list_end(list); ++i, ptr = (int*)bk_list_next(list, ptr))
	{
		if (i < 100)
			ASSERT_EQ(*ptr, i);
		else if (i >= 100)
			ASSERT_EQ(*ptr, i + 1);
	}

	bk_list_destroy(list);
}

TEST(bk_list, clear)
{
	bk_list* list = bk_list_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_list_push_back(list, &i);

	bk_list_clear(list);
	ASSERT_EQ(bk_list_get_size(list), 0);
	bk_list_destroy(list);
}