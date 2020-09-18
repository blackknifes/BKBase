//
// Created by lkq on 2020/9/18.
//

#ifndef BKBASE_BK_STREAM_H
#define BKBASE_BK_STREAM_H
#include "bk_export.h"
#include "bk_data.h"
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

    typedef struct bk_stream_struct
    {
        void* user_data;
        bk_stream_close_callback close_callback;
        bk_stream_seek_callback seek_callback;
        bk_stream_read_callback read_callback;
        bk_stream_write_callback write_callback;
    } bk_stream;

    BK_EXPORT void bk_stream_init(
        bk_stream* stream,
        void* userData,
        bk_stream_close_callback closeCallback,
        bk_stream_seek_callback seekCallback,
        bk_stream_read_callback readCallback,
        bk_stream_write_callback writeCallback);

    BK_EXPORT int bk_stream_init_with_file(bk_stream* stream, FILE* file);
	BK_EXPORT int bk_stream_init_with_filename(bk_stream* stream, const char* filename, const char* mode);
	BK_EXPORT int bk_stream_init_with_data(bk_stream* stream, const void* data, size_t size, bk_data_dtor dtor);
	BK_EXPORT int bk_stream_init_with_external_data(bk_stream* stream, bk_data* data);
#ifdef OS_WINDOWS
#include <Windows.h>
    BK_EXPORT int bk_stream_init_with_win32file(bk_stream* stream, HANDLE hFile);
#endif

    BK_EXPORT void bk_stream_close(bk_stream* stream);
    BK_EXPORT unsigned long long bk_stream_seek(bk_stream* stream, long long offset, int _where);
    BK_EXPORT size_t bk_stream_read(bk_stream* stream, void* readBuffer, size_t readSize);
    BK_EXPORT size_t bk_stream_write(bk_stream* stream, const void* data, size_t writeSize);
    BK_EXPORT bk_data* bk_stream_get_data(bk_stream* stream);
#ifdef __cplusplus
};
#endif
#endif //BKBASE_BK_STREAM_H
