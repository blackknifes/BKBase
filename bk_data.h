#ifndef __BK_DATA_H__
#define __BK_DATA_H__
#include "bk_export.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BK_DATA_STATIC ((bk_data_dtor)0)
#define BK_DATA_COPY ((bk_data_dtor)SIZE_MAX)

	typedef void(*bk_data_dtor)(void* data);
	typedef struct bk_data_struct
	{
		const void* data;
		size_t capacity;
		size_t position;
		size_t limit;
		size_t mark;
		bk_data_dtor dtor;
	} bk_data;

	//初始化data，并分配size大小的内存
	BK_EXPORT void bk_data_init(bk_data* data, size_t size);
	//使用外部缓冲初始化data
	BK_EXPORT void bk_data_init_with_data(bk_data* data, const void* buffer, size_t size, bk_data_dtor dtor);
	//销毁data
	BK_EXPORT void bk_data_destroy(bk_data* data);
	//确保内存容量(可能发生内存重新分配)
	BK_EXPORT void bk_data_reserve(bk_data* data, size_t capacity);
	//写内存到写偏移
	BK_EXPORT void bk_data_write(bk_data* data, const void* buffer, size_t size);
	//读内存到读偏移
	BK_EXPORT size_t bk_data_read(bk_data* data, void* buffer, size_t size);
	//翻转读写模式
	BK_EXPORT void bk_data_flip(bk_data* data);
	//设置limit
	BK_EXPORT void bk_data_limit(bk_data* data, size_t limitSize);
	//获取limit
	BK_EXPORT size_t bk_data_get_limit(bk_data* data);
	//获取容量
	BK_EXPORT size_t bk_data_capacity(bk_data* data);

	//设置position
	BK_EXPORT void bk_data_position(bk_data* data, size_t pos);
	//获取position
	BK_EXPORT size_t bk_data_get_position(bk_data* data);

	//设置读尺寸
	BK_EXPORT int bk_data_seek(bk_data* data, long long offset, int _where);

	//获取当前偏移
	BK_EXPORT size_t bk_data_tell(bk_data* data);

	//返回剩余可读字节 = limit - offset
	BK_EXPORT size_t bk_data_remaining(bk_data* data);
	//(压缩数据到开头)offset与limit之间的数据放置到开头
	BK_EXPORT void bk_data_compact(bk_data* data);

	//标记
	BK_EXPORT void bk_data_mark(bk_data* data);
	//还原到标记位置
	BK_EXPORT void bk_data_reset(bk_data* data);
	//还原偏移
	BK_EXPORT void bk_data_clear(bk_data* data);
	//还原偏移
	BK_EXPORT void bk_data_rewind(bk_data* data);
#ifdef __cplusplus
};
#endif

#endif
