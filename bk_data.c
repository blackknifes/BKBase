#include "bk_data.h"
#include <memory.h>
#include <malloc.h>
#include <stdio.h>

void bk_data_init(bk_data* data, size_t size)
{
	data->data = malloc(size);
	data->dtor = free;
	data->offset = data->limit = 0;
	data->size = size;
}

void bk_data_init_with_data(bk_data* data, const void* buffer, size_t size, bk_data_dtor dtor)
{
	if (!buffer)
	{
		_set_errno(EINVAL);
		return;
	}
	if (dtor == BK_DATA_COPY)
	{
		data->data = malloc(size);
		memcpy((void*)data->data, data, size);
		data->dtor = free;
	}
	else
	{
		data->data = data;
		data->dtor = dtor;
	}
	data->offset = data->limit = 0;
	data->size = size;
}

void bk_data_destroy(bk_data* data)
{
	if (data->dtor && data->data)
		data->dtor((void*)data->data);
}

void bk_data_reserve(bk_data* data, size_t capacity)
{
	if (data->size <= capacity)
		return;
	void* newData = malloc(capacity);
	memcpy(newData, data->data, data->size);
	bk_data_destroy(data);
	data->data = newData;
	data->size = capacity;
}

void bk_data_write(bk_data* data, const void* buffer, size_t size)
{
	size_t newOffset = data->offset + size;
	if (newOffset > data->size)
	{
		size_t newSize = data->size;
		do 
		{
			newSize = newSize << 1;
		} while (newSize < newOffset);
		bk_data_reserve(data, newSize);
	}
	memcpy((unsigned char*)data->data + data->offset, buffer, size);
	data->offset = newOffset;
}

size_t bk_data_read(bk_data* data, void* buffer, size_t size)
{
	size_t remainSize = data->limit - data->offset;
	if (size > remainSize)
		size = remainSize;
	memcpy(buffer, (const unsigned char*)data->data + data->offset, size);
	data->offset += size;
	return size;
}

void bk_data_flip(bk_data* data)
{
	size_t tmp = data->offset;
	data->offset = data->limit;
	data->limit = tmp;
}

void bk_data_limit(bk_data* data, size_t limitSize)
{
	if (limitSize > data->size)
		data->limit = data->size;
	else
		data->limit = limitSize;
}

int bk_data_seek(bk_data* data, long long offset, int _where)
{
	switch (_where)
	{
	case SEEK_SET:
		if (offset > 0)
			data->offset = (size_t)((unsigned long long)offset > data->limit ? data->limit : offset);
		else
			data->offset = 0;
		break;
	case SEEK_CUR:
		if (offset > 0)
		{
			if ((unsigned long long)offset > data->limit - data->offset)
				data->offset = data->limit;
			else
				data->offset += offset;
		}
		else
		{
			if ((size_t)(-offset) > data->offset)
				data->offset = 0;
			else
				data->offset += offset;
		}
		break;
	case SEEK_END:
		if (offset > 0)
			data->offset = data->limit;
		else
		{
			if ((size_t)(-offset) > data->limit)
				data->offset = 0;
			else
				data->offset = data->limit + offset;
		}
		break;
	default:
		return EINVAL;
	}
	return 0;
}

size_t bk_data_tell(bk_data* data)
{
	return data->offset;
}
