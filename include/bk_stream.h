//
// Created by lkq on 2020/9/18.
//

#ifndef BKBASE_BK_STREAM_H
#define BKBASE_BK_STREAM_H
#include "bk_export.h"
#include "bk_buffer.h"
#include <intsafe.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef void(*bk_stream_close_callback)(void* userData);
    typedef unsigned long long(*bk_stream_seek_callback)(void* userData, long long offset, int _where);
    typedef size_t(*bk_stream_read_callback)(void* userData, void* readBuffer, size_t readSize);
    typedef size_t(*bk_stream_write_callback)(void* userData, const void* data, size_t writeSize);
    typedef bk_bool(*bk_stream_eof_callback)(void* userData);

    //流接口
    struct bk_stream_interface_struct;
    typedef struct bk_stream_interface_struct bk_stream_interface;

    typedef struct bk_stream_struct
    {
        void* user_data;
        bk_stream_interface* stream_interface;
    } bk_stream;

    //创建流接口
	BK_EXPORT bk_stream_interface* bk_stream_interface_create(
		bk_stream_close_callback closeCallback,
		bk_stream_seek_callback seekCallback,
		bk_stream_read_callback readCallback,
		bk_stream_write_callback writeCallback,
		bk_stream_eof_callback eof_callback);
    //销毁流接口(不应将已加载到stream上的接口传递给此参数)
    BK_EXPORT void bk_stream_interface_destroy(bk_stream_interface* _interface);

    //初始化流对象
    BK_EXPORT void bk_stream_init(
        bk_stream* stream,
        void* userData,
        bk_stream_interface* _interface);

    BK_EXPORT int bk_stream_init_with_file(bk_stream* stream, FILE* file);
	BK_EXPORT int bk_stream_init_with_filename(bk_stream* stream, const char* filename, const char* mode);
	BK_EXPORT int bk_stream_init_with_buffer(bk_stream* stream, void* data, size_t size, bk_buffer_dtor dtor);
	BK_EXPORT int bk_stream_init_with_external_buffer(bk_stream* stream, bk_buffer* data);
#ifdef OS_WINDOWS
#include <Windows.h>
    BK_EXPORT int bk_stream_init_with_win32file(bk_stream* stream, HANDLE hFile);
#endif

    //关闭流(将自动销毁接口，并关闭内部对象)
    BK_EXPORT void bk_stream_close(bk_stream* stream);
    //移动流指针
    BK_EXPORT unsigned long long bk_stream_seek(bk_stream* stream, long long offset, int _where);
    //从流读入数据
    BK_EXPORT size_t bk_stream_read(bk_stream* stream, void* readBuffer, size_t readSize);
    //从流写出数据
    BK_EXPORT size_t bk_stream_write(bk_stream* stream, const void* data, size_t writeSize);
    //获取流中的buffer对象
    BK_EXPORT bk_buffer* bk_stream_get_buffer(bk_stream* stream);
    //流eof测试
    BK_EXPORT bk_bool bk_stream_eof(bk_stream* stream);

#ifdef __cplusplus
};
#endif
#endif //BKBASE_BK_STREAM_H
