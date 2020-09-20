#include "bk_data.h"

typedef struct bk_data_internal_struct
{
	unsigned int ref;
	void* data;
	size_t size;
} bk_data_internal;

typedef struct bk_data_struct
{
	bk_data_internal* internal_data;		//内部实现
	void* data;								//数据指针
	size_t size;							//数据大小
} bk_data;

static bk_data_internal* bk_data_internal_create(size_t size)
{
	bk_data_internal* data = (bk_data_internal*)malloc(sizeof(bk_data_internal));
	data->data = malloc(size);
	data->size = size;
	data->ref = 0;
	return data;
}

static bk_data_internal* bk_data_internal_create_with_data(const void* buf, size_t size)
{
	bk_data_internal* data = (bk_data_internal*)malloc(sizeof(bk_data_internal));
	data->data = malloc(size);
	data->size = size;
	data->ref = 0;
	memcpy(data->data, buf, size);
	return data;
}

static void bk_data_internal_destroy(bk_data_internal* data)
{
	if(--data->ref == 0)
		free(data);
}

bk_data* bk_data_create(size_t size)
{
	bk_data* data = (bk_data*)malloc(sizeof(bk_data));
	bk_data_internal* _internal = bk_data_internal_create(size);
	data->internal_data = _internal;
	data->data = _internal->data;
	data->size = _internal->size;
	++_internal->ref;
	return data;
}

bk_data* bk_data_create_with_sub(bk_data* data, size_t offset, size_t size)
{
	++data->internal_data->ref;
	bk_data* newData = (bk_data*)malloc(sizeof(bk_data));
	newData->internal_data = data->internal_data;

	size_t availSize = data->size - offset;
	if (offset > data->size)
		offset = data->size;
	if (size > availSize)
		size = availSize;

	newData->data = (unsigned char*)data->data + offset;
	newData->size = size;
	return newData;
}

void bk_data_destroy(bk_data* data)
{
	bk_data_internal_destroy(data->internal_data);
	free(data);
}

size_t bk_data_get_size(const bk_data* data)
{
	return data->size;
}

const void* bk_data_get_data(const bk_data* data)
{
	return data->data;
}

size_t bk_data_write(bk_data* data, const void* buf, size_t offset, size_t size)
{
	if (offset >= size)
		return 0;
	size_t writableSize = data->size - offset;
	if (size > writableSize)
		size = writableSize;

	if (data->internal_data->ref > 1)
	{//发生写时拷贝
		bk_data_internal* pInternal = bk_data_internal_create_with_data(data->data, data->size);
		bk_data_internal_destroy(data->internal_data);
		data->internal_data = pInternal;
		++pInternal->ref;
	}

	memcpy((unsigned char*)data->data + offset, buf, size);
	return size;
}

size_t bk_data_read(const bk_data* data, void* buf, size_t offset, size_t size)
{
	if (offset >= size)
		return 0;
	size_t readableSize = data->size - offset;
	if (size > readableSize)
		size = readableSize;
	memcpy(buf, (const unsigned char*)data->data + offset, size);
	return size;
}
