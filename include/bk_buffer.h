#ifndef __BK_BUFFER_H__
#define __BK_BUFFER_H__
#include "bk_export.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BK_BUFFER_STATIC ((bk_buffer_dtor)0)
#define BK_BUFFER_COPY ((bk_buffer_dtor)SIZE_MAX)

	typedef void(*bk_buffer_dtor)(void* data);

	struct bk_buffer_struct;
	typedef struct bk_buffer_struct bk_buffer;

	//初始化data，并分配size大小的内存
	BK_EXPORT bk_buffer* bk_buffer_create(size_t size);
	//使用外部缓冲初始化data
	BK_EXPORT bk_buffer* bk_buffer_create_with_data(void* buffer, size_t size, bk_buffer_dtor dtor);
	//销毁data
	BK_EXPORT void bk_buffer_destroy(bk_buffer* data);
	//确保内存容量(可能发生内存重新分配)
	BK_EXPORT void bk_buffer_reserve(bk_buffer* data, size_t capacity);
	//写内存到写偏移
	BK_EXPORT void bk_buffer_write(bk_buffer* data, const void* buffer, size_t size);
	//读内存到读偏移
	BK_EXPORT size_t bk_buffer_read(bk_buffer* data, void* buffer, size_t size);
	//翻转读写模式
	BK_EXPORT void bk_buffer_flip(bk_buffer* data);
	//设置limit
	BK_EXPORT void bk_buffer_limit(bk_buffer* data, size_t limitSize);
	//获取limit
	BK_EXPORT size_t bk_buffer_get_limit(bk_buffer* data);
	//获取容量
	BK_EXPORT size_t bk_buffer_get_capacity(bk_buffer* data);

	//设置position
	BK_EXPORT void bk_buffer_position(bk_buffer* data, size_t pos);
	//获取position
	BK_EXPORT size_t bk_buffer_get_position(bk_buffer* data);

	//设置读尺寸
	BK_EXPORT int bk_buffer_seek(bk_buffer* data, long long offset, int _where);

	//获取当前偏移
	BK_EXPORT size_t bk_buffer_tell(bk_buffer* data);

	//返回剩余可读字节 = limit - offset
	BK_EXPORT size_t bk_buffer_remaining(bk_buffer* data);
	//(压缩数据到开头)offset与limit之间的数据放置到开头
	BK_EXPORT void bk_buffer_compact(bk_buffer* data);

	//标记
	BK_EXPORT void bk_buffer_mark(bk_buffer* data);
	//还原到标记位置
	BK_EXPORT void bk_buffer_reset(bk_buffer* data);
	//还原偏移
	BK_EXPORT void bk_buffer_clear(bk_buffer* data);
	//还原偏移
	BK_EXPORT void bk_buffer_rewind(bk_buffer* data);
	//eof测试
	BK_EXPORT bk_bool bk_buffer_eof(bk_buffer* data);
#ifdef __cplusplus
};
#endif

#endif
