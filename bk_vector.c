#include "bk_vector.h"
#include <assert.h>
#include <malloc.h>
#include <memory.h>

#define ALIGN(size) (((size) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

typedef struct bk_vector_struct
{
	size_t value_size;
	size_t size;
	size_t capacity;
	void* data;
	void(*dtor)(void*);
}bk_vector;

static void* bk_vector_get_data_noassert(bk_vector* vec, size_t index)
{
	return (unsigned char*)vec->data + (index * vec->value_size);
}

bk_vector* bk_vector_create(size_t valSize, void(*dtor)(void*))
{
	if (valSize == 0)
	{
		_set_errno(EINVAL);
		return 0;
	}
	bk_vector* vec = (bk_vector*)malloc(sizeof(bk_vector));
	vec->value_size = ALIGN(valSize);
	vec->size = vec->capacity = 0;
	vec->data = 0;
	vec->dtor = dtor;
	return vec;
}

void bk_vector_destroy(bk_vector* vec)
{
	if (vec->dtor)
	{
		for (size_t i = 0; i < vec->size; ++i)
			vec->dtor(bk_vector_get_data(vec, i));
	}
	if (vec->data)
		free(vec->data);
	free(vec);
}

void* bk_vector_get_array_pointer(bk_vector* vec)
{
	return vec->data;
}

void bk_vector_reserve(bk_vector* vec, size_t capacity)
{
	if (capacity <= vec->capacity)
		return;
	if (!vec->data)
		vec->data = malloc(vec->value_size * capacity);
	else
		vec->data = realloc(vec->data, vec->value_size * capacity);
	vec->capacity = capacity;
}

void bk_vector_resize(bk_vector* vec, size_t size)
{
	bk_vector_reserve(vec, size);
	if (vec->dtor && size < vec->size)
	{
		for (size_t i = size; i < vec->size; ++i)
			vec->dtor(bk_vector_get_data(vec, i));
	}
	vec->size = size;
}

size_t bk_vector_get_capcity(bk_vector* vec)
{
	return vec->capacity;
}

size_t bk_vector_get_size(bk_vector* vec)
{
	return vec->size;
}

void* bk_vector_get_data(bk_vector* vec, size_t index)
{
	assert(index < vec->size);
	return bk_vector_get_data_noassert(vec, index);
}

void* bk_vector_get_front(bk_vector* vec)
{
	return bk_vector_get_data(vec, 0);
}

void* bk_vector_get_back(bk_vector* vec)
{
	return bk_vector_get_data(vec, vec->size - 1);
}

void bk_vector_push_back(bk_vector* vec, const void* _data)
{
	if (vec->size == vec->capacity)
		bk_vector_reserve(vec, vec->capacity << 1);
	++vec->size;
	memcpy(bk_vector_get_back(vec), _data, vec->value_size);
}

void bk_vector_pop_back(bk_vector* vec)
{
	void* ptr = bk_vector_get_back(vec);
	if (vec->dtor)
		vec->dtor(ptr);
	--vec->size;
}

void bk_vector_insert(bk_vector* vec, const void* _data, size_t index)
{
	assert(index <= vec->size);
	if (vec->size == vec->capacity)
		bk_vector_reserve(vec, vec->capacity << 1);
	size_t moveCount = vec->size - index;
	memmove(bk_vector_get_data_noassert(vec, index + 1), bk_vector_get_data_noassert(vec, index), vec->value_size * moveCount);
	memcpy(bk_vector_get_data_noassert(vec, index), _data, vec->value_size);
	++vec->size;
}

void bk_vector_erase(bk_vector* vec, size_t index)
{
	if(vec->dtor)
		vec->dtor(bk_vector_get_data(vec, index));
	size_t moveCount = vec->size - index - 1;
	memcpy(bk_vector_get_data_noassert(vec, index), bk_vector_get_data_noassert(vec, index + 1), moveCount * vec->value_size);
	--vec->size;
}
