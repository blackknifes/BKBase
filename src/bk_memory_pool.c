#include "bk_memory_pool.h"
#ifdef OS_WINDOWS
#include <Windows.h>
#endif

struct bk_memory_pool_struct
{
	void** pool;
	size_t pool_size;
	size_t element_size;
#ifdef OS_WINDOWS
	CRITICAL_SECTION mutex;
#endif
};

bk_memory_pool* bk_memory_pool_create(size_t elementSize)
{
	assert(elementSize != 0);
	elementSize = BK_ALIGN(elementSize);
	bk_memory_pool* pool = (bk_memory_pool*)malloc(sizeof(bk_memory_pool));
	pool->pool = BK_NULL;
	pool->pool_size = 0;
	pool->element_size = elementSize;
	return pool;
}

bk_memory_pool* bk_memory_pool_create_threadsafe(size_t elementSize)
{

}

void bk_memory_pool_destroy(bk_memory_pool* pool)
{

}
