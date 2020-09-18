#include "bk_stream.h"

typedef struct
{
	const void* data;
	size_t size;
	size_t offset;
} bk_data_user_data;

//-------标准文件操作-------
static void bk_file_close(void* userData)
{
	fclose((FILE*)userData);
}

static unsigned long long bk_file_seek(void* userData, long long offset, int _where)
{
	int result = _fseeki64((FILE*)userData, offset, _where);
	switch (result)
	{
	case -1:
		return ULONGLONG_MAX;
	case 0:
		_set_errno(result);
		return ULONGLONG_MAX;
	default:
		break;
	}
	return _ftelli64((FILE*)userData);
}

static size_t bk_file_read(void* userData, void* readBuffer, size_t readSize)
{
	return fread(readBuffer, 1, readSize, (FILE*)userData);
}

static size_t bk_file_write(void* userData, const void* readBuffer, size_t writeSize)
{
	return fwrite(readBuffer, 1, writeSize, (FILE*)userData);
}
//-------标准文件操作-------

//-------内存操作-------
static unsigned long long bk_data_seek(void* userData, long long offset, int _where)
{
	bk_data_user_data* data = (bk_data_user_data*)userData;

	size_t newOffset;
	switch (_where)
	{
	case SEEK_SET:
		newOffset = offset > 0? offset : 0;
		break;
	case SEEK_CUR:
	{
		if (offset > 0)
			newOffset = data->offset + offset;
		else
		{
			if (-offset > data->offset)
				newOffset = 0;
			else
				newOffset = data->offset + offset;
		}
		break;
	}
	case SEEK_END:
	{
		break;
	}
	default:
		_set_errno(EINVAL);
		return ULONGLONG_MAX;
	}

	return data->offset;
}

static size_t bk_data_read(void* userData, void* readBuffer, size_t readSize)
{
	
}

static size_t bk_data_write(void* userData, const void* readBuffer, size_t writeSize)
{
	
}
//-------内存操作-------

void bk_stream_init(
	bk_stream* stream,
	void* userData,
	bk_stream_close_callback closeCallback,
	bk_stream_seek_callback seekCallback,
	bk_stream_read_callback readCallback,
	bk_stream_write_callback writeCallback)
{
	stream->user_data = userData;
	stream->close_callback = closeCallback;
	stream->seek_callback = seekCallback;
	stream->read_callback = readCallback;
	stream->write_callback = writeCallback;
}

int bk_stream_init_with_file(bk_stream* stream, FILE* file)
{
	if (!file)
		return EINVAL;
	bk_stream_init(stream, file, bk_file_close, bk_file_seek, bk_file_read, bk_file_write);
	return 0;
}

int bk_stream_init_with_filename(bk_stream* stream, const char* filename, const char* mode)
{
	FILE* f = fopen(filename, mode);
	if (!f)
		return errno;

	int result = bk_stream_init_with_file(stream, f);
	if(result != 0)
		fclose(f);
	return result;
}

int bk_stream_init_with_data(bk_stream* stream, const void* data, size_t size)
{

}
#ifdef OS_WINDOWS
#include <Windows.h>
int bk_stream_init_with_win32file(bk_stream* stream, HANDLE hFile);
#endif