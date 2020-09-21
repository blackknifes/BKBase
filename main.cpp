#include "bk_hashmap.h"
#include "bk_list.h"
#include "bk_stream.h"
#include "bk_vector.h"
#include <stdio.h>
#include <vector>
#include <algorithm>

int main()
{
    system("chcp 65001");
//     bk_stream stream;
// 
//     HANDLE hFile = CreateFileA("D:/test.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
//     bk_stream_init_with_win32file(&stream, hFile);
//     bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
//     bk_stream_close(&stream);
// 
//     bk_stream_init_with_filename(&stream, "D:/test2.txt", "wb+");
// 	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
// 	bk_stream_close(&stream);
// 
// 	bk_stream_init_with_buffer(&stream, nullptr, 0x4000, BK_BUFFER_COPY);
// 	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
// 
//     bk_buffer* data = bk_stream_get_buffer(&stream);
//     ((unsigned char*)data->data)[data->position] = 0;
//     printf("%s\n", (char*)data->data);
// 	bk_stream_close(&stream);
// 
//     bk_buffer tmpData;
//     bk_buffer_init(&tmpData, 1024);
// 
// 	bk_stream_init_with_external_buffer(&stream, &tmpData);
// 	bk_stream_write(&stream, "Test测试", sizeof("Test测试") - 1);
// 	((unsigned char*)tmpData.data)[tmpData.position] = 0;
// 	printf("%s\n", (char*)tmpData.data);
// 	bk_stream_close(&stream);


//     bk_vector* vec = bk_vector_create(sizeof(size_t), nullptr);
//     for (size_t i = 0; i < 1024; ++i)
//         bk_vector_push_back(vec, &i);
// 
//     for (size_t i = 0; i < bk_vector_get_size(vec); ++i)
//         printf("%zu\n", *(size_t*)bk_vector_get_data(vec, i));
//     bk_vector_destroy(vec);
//     system("cls");
// 
// 	bk_list* list = bk_list_create(sizeof(size_t), nullptr);
// 	for (size_t i = 0; i < 512; ++i)
// 		bk_list_push_back(list, &i);
// 	for (size_t i = 512; i < 1024; ++i)
// 		bk_list_push_front(list, &i);
// 
//     void* ptr = bk_list_get_front(list);
// 	while (ptr)
// 	{
//         printf("%zu\n", *(size_t*)ptr);
//         ptr = bk_list_get_next(ptr);
// 	}
// 	bk_list_destroy(list);

    bk_hashmap* map = bk_hashmap_create_with_strkey(free, nullptr);

    for (size_t i = 0; i < 1024; ++i)
    {
        char* buf = (char*)malloc(16);
        sprintf_s(buf, 16, "%d", (int)i);
        bk_hashmap_insert(map, buf, (void*)i);
    }

    std::vector<size_t> vals;

    auto itor = bk_hashmap_iterator_begin(map);
    while (bk_hashmap_iterator_is_valid(&itor))
    {
		const char* key = (const char*)bk_hashmap_iterator_get_key(&itor);
		size_t val = (size_t)bk_hashmap_iterator_get_value(&itor);
	//	printf("%s: %d\n", key, (int)val);
        bk_hashmap_iterator_next(&itor);
        vals.push_back(val);
    }
    std::sort(vals.begin(), vals.end());
    for (size_t val: vals)
    {
        printf("%zu\n", val);
    }

    bk_hashmap_destroy(map);

    return 0;
}
