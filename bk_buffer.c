#include "bk_buffer.h"
#include <memory.h>
#include <malloc.h>
#include <stdio.h>

void bk_buffer_init(bk_buffer* data, size_t size)
{
	data->data = malloc(size);
	data->dtor = free;
	data->position = data->limit = 0;
	data->capacity = size;
	data->mark = SIZE_MAX;
}

void bk_buffer_init_with_data(bk_buffer* data, void* buffer, size_t size, bk_buffer_dtor dtor)
{
	if (!buffer)
	{
		_set_errno(EINVAL);
		return;
	}
	if (dtor == BK_BUFFER_COPY)
	{
		data->data = malloc(size);
		memcpy(data->data, data, size);
		data->dtor = free;
	}
	else
	{
		data->data = (void*)data;
		data->dtor = dtor;
	}
	data->position = data->limit = 0;
	data->capacity = size;
	data->mark = SIZE_MAX;
}

void bk_buffer_destroy(bk_buffer* data)
{
	if (data->dtor && data->data)
		data->dtor(data->data);
}

void bk_buffer_reserve(bk_buffer* data, size_t capacity)
{
	if (data->capacity <= capacity)
		return;
	void* newData = malloc(capacity);
	memcpy(newData, data->data, data->capacity);
	bk_buffer_destroy(data);
	data->data = newData;
	data->capacity = capacity;
}

void bk_buffer_write(bk_buffer* data, const void* buffer, size_t size)
{
	size_t newOffset = data->position + size;
	if (newOffset > data->capacity)
	{
		size_t newSize = data->capacity;
		do 
		{
			newSize = newSize << 1;
		} while (newSize < newOffset);
		bk_buffer_reserve(data, newSize);
	}
	memcpy((unsigned char*)data->data + data->position, buffer, size);
	data->position = newOffset;
}

size_t bk_buffer_read(bk_buffer* data, void* buffer, size_t size)
{
	size_t remainSize = data->limit - data->position;
	if (size > remainSize)
		size = remainSize;
	memcpy(buffer, (const unsigned char*)data->data + data->position, size);
	data->position += size;
	return size;
}

void bk_buffer_flip(bk_buffer* data)
{
	data->limit = data->position;
	data->position = 0;
	data->mark = SIZE_MAX;
}

void bk_buffer_limit(bk_buffer* data, size_t limitSize)
{
	if (limitSize > data->capacity)
		data->limit = data->capacity;
	else
		data->limit = limitSize;

	if (data->mark != SIZE_MAX && data->mark > data->limit)
		data->mark = SIZE_MAX;
}

size_t bk_buffer_get_limit(bk_buffer* data)
{
	return data->limit;
}

size_t bk_buffer_capacity(bk_buffer* data)
{
	return data->capacity;
}

void bk_buffer_position(bk_buffer* data, size_t pos)
{
	data->position = pos > data->limit ? data->limit : pos;
	if (data->mark != SIZE_MAX && data->mark > data->position)
		data->mark = SIZE_MAX;
}

size_t bk_buffer_get_position(bk_buffer* data)
{
	return data->position;
}

int bk_buffer_seek(bk_buffer* data, long long offset, int _where)
{
	switch (_where)
	{
	case SEEK_SET:
		if (offset > 0)
			data->position = (size_t)((unsigned long long)offset > data->limit ? data->limit : offset);
		else
			data->position = 0;
		break;
	case SEEK_CUR:
		if (offset > 0)
		{
			if ((unsigned long long)offset > data->limit - data->position)
				data->position = data->limit;
			else
				data->position += offset;
		}
		else
		{
			if ((size_t)(-offset) > data->position)
				data->position = 0;
			else
				data->position += offset;
		}
		break;
	case SEEK_END:
		if (offset > 0)
			data->position = data->limit;
		else
		{
			if ((size_t)(-offset) > data->limit)
				data->position = 0;
			else
				data->position = data->limit + offset;
		}
		break;
	default:
		return EINVAL;
	}
	return 0;
}

size_t bk_buffer_tell(bk_buffer* data)
{
	return data->position;
}

size_t bk_buffer_remaining(bk_buffer* data)
{
	return data->limit - data->position;
}

void bk_buffer_compact(bk_buffer* data)
{
	memcpy(data->data, (const unsigned char*)data->data + data->position, data->limit - data->position);
	data->position = data->limit - data->position;
	data->limit = data->capacity;
}

void bk_buffer_mark(bk_buffer* data)
{
	data->mark = data->position - 1;
}

void bk_buffer_reset(bk_buffer* data)
{
	data->position = data->mark + 1;
}

void bk_buffer_clear(bk_buffer* data)
{
	data->position = 0;
	data->limit = data->capacity;
	data->mark = SIZE_MAX;
}

void bk_buffer_rewind(bk_buffer* data)
{
	data->position = 0;
	data->mark = SIZE_MAX;
}

int bk_buffer_eof(bk_buffer* data)
{
	return data->position == data->limit;
}
