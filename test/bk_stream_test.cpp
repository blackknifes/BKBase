#include "bk_stream.h"
#include "file_test_util.h"
#include "gtest/gtest.h"

TEST(bk_stream, file)
{
	bk_stream stream;
	int result = bk_stream_init_with_filename(&stream, "test.txt", "wb+");
	ASSERT_EQ(result, BK_ERROR_OK);
	if(result == BK_ERROR_OK)
	{
		ASSERT_EQ(bk_stream_write(&stream, "0123456", 7), 7);
		bk_stream_seek(&stream, 0, SEEK_SET);
		bk_stream_write(&stream, "ab", 2);
		bk_stream_close(&stream);
	}
	char* str = bk_test_read_file("test.txt");
	ASSERT_STRCASEEQ(str, "ab23456");
	bk_test_read_file_free(str);

	char buf[1024] = {};
	result = bk_stream_init_with_filename(&stream, "test.txt", "rb");
	ASSERT_EQ(result, BK_ERROR_OK);
	if (result == BK_ERROR_OK)
	{
		ASSERT_EQ(bk_stream_read(&stream, buf, 1024), 7);
		ASSERT_TRUE(bk_stream_eof(&stream));
		bk_stream_close(&stream);
	}
	ASSERT_STRCASEEQ(buf, "ab23456");

	bk_test_delete_file("test.txt");
}

TEST(bk_stream, buffer)
{
	bk_stream stream;
	char buf[1024] = {};
	int result = bk_stream_init_with_buffer(&stream, nullptr, 1024, BK_BUFFER_COPY);
	ASSERT_EQ(result, BK_ERROR_OK);
	if (result != BK_ERROR_OK)
		return;
	ASSERT_EQ(bk_stream_write(&stream, "0123456", 7), 7);
	bk_stream_seek(&stream, 0, SEEK_SET);
	bk_stream_write(&stream, "ab", 2);
	
	bk_buffer* pBuffer = bk_stream_get_buffer(&stream);
	bk_buffer_reset(pBuffer);
	bk_buffer_limit(pBuffer, 7);
	ASSERT_EQ(bk_buffer_read(pBuffer, buf, 1024), 7);
	bk_stream_close(&stream);
	ASSERT_STRCASEEQ(buf, "ab23456");
	memset(buf, 0, sizeof(buf));

	bk_buffer* tmpBuffer = bk_buffer_create(1024);
	result = bk_stream_init_with_external_buffer(&stream, tmpBuffer);
	ASSERT_EQ(result, BK_ERROR_OK);
	ASSERT_EQ(bk_stream_write(&stream, "0123456", 7), 7);
	bk_stream_seek(&stream, 0, SEEK_SET);
	bk_stream_write(&stream, "ab", 2);
	bk_stream_close(&stream);

	bk_buffer_reset(tmpBuffer);
	bk_buffer_limit(tmpBuffer, 7);
	ASSERT_EQ(bk_buffer_read(tmpBuffer, buf, 1024), 7);
	ASSERT_STRCASEEQ(buf, "ab23456");
	bk_buffer_destroy(tmpBuffer);
}

#ifdef OS_WINDOWS
TEST(bk_stream, win32file)
{
	bk_stream stream;
	HANDLE hFile = CreateFileA("test.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	ASSERT_NE(hFile, INVALID_HANDLE_VALUE);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetEndOfFile(hFile);
		ASSERT_EQ(bk_stream_init_with_win32file(&stream, hFile), BK_ERROR_OK);
		ASSERT_EQ(bk_stream_write(&stream, "0123456", 7), 7);
		bk_stream_seek(&stream, 0, SEEK_SET);
		bk_stream_write(&stream, "ab", 2);
		bk_stream_close(&stream);
	}
	char* str = bk_test_read_file("test.txt");
	ASSERT_STRCASEEQ(str, "ab23456");
	bk_test_read_file_free(str);

	hFile = CreateFileA("test.txt", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	ASSERT_NE(hFile, INVALID_HANDLE_VALUE);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		char buf[1024] = {};
		ASSERT_EQ(bk_stream_init_with_win32file(&stream, hFile), BK_ERROR_OK);
		ASSERT_EQ(bk_stream_read(&stream, buf, 1024), 7);
		bk_stream_close(&stream);
		ASSERT_STRCASEEQ(buf, "ab23456");
	}

	bk_test_delete_file("test.txt");
}
#endif