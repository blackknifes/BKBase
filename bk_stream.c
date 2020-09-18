#include "bk_stream.h"

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

static size_t bk_file_write(void* userData, const void* data, size_t writeSize)
{
	return fwrite(data, 1, writeSize, (FILE*)userData);
}
//-------标准文件操作-------

//-------内存操作-------
static void bk_stream_data_close(void* userData)
{
	bk_data* data = (bk_data*)userData;
	bk_data_destroy(data);
	free(data);
}

static void bk_stream_data_close_no_free(void* userData)
{
	bk_data* data = (bk_data*)userData;
	bk_data_destroy(data);
	memset(data, 0, sizeof(bk_data));
}

static unsigned long long bk_stream_data_seek(void* userData, long long offset, int _where)
{
	bk_data* data = (bk_data*)userData;
	bk_data_seek(data, offset, _where);
	return bk_data_tell(data);
}

static size_t bk_stream_data_read(void* userData, void* readBuffer, size_t readSize)
{
	return bk_data_read((bk_data*)userData, readBuffer, readSize);
}

static size_t bk_stream_data_write(void* userData, const void* writeBuffer, size_t writeSize)
{
	bk_data_write((bk_data*)userData, writeBuffer, writeSize);
	return writeSize;
}
//-------内存操作-------

//-------win32文件操作-------
#ifdef OS_WINDOWS
static void bk_win32file_close(void* userData)
{
	CloseHandle(userData);
}

static unsigned long long bk_win32file_seek(void* userData, long long offset, int _where)
{
	DWORD pos;
	switch (_where)
	{
	case SEEK_SET:
		pos = FILE_BEGIN;
		break;
	case SEEK_CUR:
		pos = FILE_CURRENT;
		break;
	case SEEK_END:
		pos = FILE_END;
		break;
	default:
		_set_errno(EINVAL);
		return ULONGLONG_MAX;
	}

	LARGE_INTEGER largeOffset;
	LARGE_INTEGER currentOffset;
	largeOffset.QuadPart = offset;
	currentOffset.QuadPart = 0;
	if (!SetFilePointerEx(userData, largeOffset, &currentOffset, pos))
	{
		_set_errno(EFAULT);
		return ULONGLONG_MAX;
	}
	return (unsigned long long)currentOffset.QuadPart;
}

static size_t bk_win32file_read(void* userData, void* readBuffer, size_t readSize)
{
	DWORD dwSize = 0;
	ReadFile(userData, readBuffer, (DWORD)readSize, &dwSize, 0);
	return dwSize;
}

static size_t bk_win32file_write(void* userData, const void* data, size_t writeSize)
{
	DWORD dwSize = 0;
	WriteFile(userData, data, (DWORD)writeSize, &dwSize, 0);
	return dwSize;
}
#endif
//-------win32文件操作-------


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

int bk_stream_init_with_data(bk_stream* stream, const void* data, size_t size, bk_data_dtor dtor)
{
	if (!data && dtor != BK_DATA_COPY)
		return EINVAL;
	bk_data* dataUserData = (bk_data*)malloc(sizeof(bk_data));

	if (dtor == BK_DATA_COPY && !data)
		bk_data_init(dataUserData, size);
	else
		bk_data_init_with_data(dataUserData, data, size, dtor);
	bk_data_limit(dataUserData, size);

	bk_stream_init(
		stream, 
		dataUserData,
		bk_stream_data_close,
		bk_stream_data_seek,
		bk_stream_data_read,
		bk_stream_data_write);
	return 0;
}

int bk_stream_init_with_external_data(bk_stream* stream, bk_data* data)
{
	if (!data)
		return EINVAL;
	bk_stream_init(
		stream,
		data,
		bk_stream_data_close_no_free,
		bk_stream_data_seek,
		bk_stream_data_read,
		bk_stream_data_write);
	return 0;
}

#ifdef OS_WINDOWS
#include <Windows.h>
int bk_stream_init_with_win32file(bk_stream* stream, HANDLE hFile)
{
	if (!hFile)
		return EINVAL;
	bk_stream_init(
		stream, 
		hFile, 
		bk_win32file_close, 
		bk_win32file_seek, 
		bk_win32file_read, 
		bk_win32file_write);
	return 0;
}

void bk_stream_close(bk_stream* stream)
{
	if (stream->close_callback)
		stream->close_callback(stream->user_data);
}

unsigned long long bk_stream_seek(bk_stream* stream, long long offset, int _where)
{
	return stream->seek_callback(stream->user_data, offset, _where);
}

size_t bk_stream_read(bk_stream* stream, void* readBuffer, size_t readSize)
{
	return stream->read_callback(stream->user_data, readBuffer, readSize);
}

size_t bk_stream_write(bk_stream* stream, const void* data, size_t writeSize)
{
	return stream->write_callback(stream->user_data, data, writeSize);
}

bk_data* bk_stream_get_data(bk_stream* stream)
{
	return (bk_data*)stream->user_data;
}

#endif
