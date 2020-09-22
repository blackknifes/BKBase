#ifndef __BK_MEMORY_POOL_H__
#define __BK_MEMORY_POOL_H__
#include "bk_export.h"
#include "bk_types.h"
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
	struct bk_memory_pool_struct;
	typedef struct bk_memory_pool_struct bk_memory_pool;

	bk_memory_pool* bk_memory_pool_create(size_t elementSize);
	bk_memory_pool* bk_memory_pool_create_threadsafe(size_t elementSize);
	void bk_memory_pool_destroy(bk_memory_pool* pool);

	void bk_memory_pool_push(void* mem);
	void* bk_memory_pool_pop();

	void bk_memory_pool_clear();
#ifdef __cplusplus
};
#endif

#endif