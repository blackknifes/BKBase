#include "gtest/gtest.h"
#include "bk_vector.h"

TEST(bk_vector, getter)
{
	bk_vector* vec = bk_vector_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_vector_push_back(vec, &i);
	for (int i = 0; i < 128; ++i)
		ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i);
	ASSERT_EQ(*(int*)bk_vector_get_front(vec), 0);
	ASSERT_EQ(*(int*)bk_vector_get_back(vec), 127);
	ASSERT_EQ(bk_vector_get_size(vec), 128);

	int* itor = (int*)bk_vector_begin(vec);
	for (int i = 0; itor != bk_vector_end(vec); ++i, itor = (int*)bk_vector_next(vec, itor))
		ASSERT_EQ(*itor, i);

	bk_vector_destroy(vec);
}

TEST(bk_vector, insert)
{
	bk_vector* vec = bk_vector_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
	{
		if(i == 100)
			continue;
		bk_vector_push_back(vec, &i);
	}
	for (int i = 0; i < 127; ++i)
	{
		if(i < 100)
			ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i);
		else if(i >= 100)
			ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i + 1);
	}

	ASSERT_EQ(bk_vector_get_size(vec), 127);
	int val = 100;
	bk_vector_insert(vec, &val, 100);
	for (int i = 0; i < 128; ++i)
		ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i);
	ASSERT_EQ(bk_vector_get_size(vec), 128);
	bk_vector_destroy(vec);
}

TEST(bk_vector, erase)
{
	bk_vector* vec = bk_vector_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_vector_push_back(vec, &i);
	bk_vector_erase(vec, 100);
	for (int i = 0; i < 127; ++i)
	{
		if (i < 100)
			ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i);
		else if (i >= 100)
			ASSERT_EQ(*(int*)bk_vector_get_data(vec, i), i + 1);
	}
	ASSERT_EQ(bk_vector_get_size(vec), 127);

	bk_vector_destroy(vec);
}

TEST(bk_vector, clear)
{
	bk_vector* vec = bk_vector_create(sizeof(int), BK_NULL);
	for (int i = 0; i < 128; ++i)
		bk_vector_push_back(vec, &i);

	bk_vector_clear(vec);
	ASSERT_EQ(bk_vector_get_size(vec), 0);
	bk_vector_destroy(vec);
}