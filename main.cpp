#include <stdio.h>
#include "bk_stream.h"

int main()
{
    system("chcp 65001");
    bk_stream stream;

    HANDLE hFile = CreateFileA("D:/test.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    bk_stream_init_with_win32file(&stream, hFile);
    bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
    bk_stream_close(&stream);

    bk_stream_init_with_filename(&stream, "D:/test2.txt", "wb+");
	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
	bk_stream_close(&stream);

	bk_stream_init_with_buffer(&stream, nullptr, 0x4000, BK_BUFFER_COPY);
	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);

    bk_buffer* data = bk_stream_get_buffer(&stream);
    ((unsigned char*)data->data)[data->position] = 0;
    printf("%s\n", (char*)data->data);
	bk_stream_close(&stream);

    bk_buffer tmpData;
    bk_buffer_init(&tmpData, 1024);

	bk_stream_init_with_external_buffer(&stream, &tmpData);
	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
	((unsigned char*)tmpData.data)[tmpData.position] = 0;
	printf("%s\n", (char*)tmpData.data);
	bk_stream_close(&stream);

    return 0;
}
