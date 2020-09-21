#include "bk_stream.h"


typedef struct bk_stream_interface_struct
{
	struct bk_stream_interface_struct** stored;
	unsigned int ref;
	bk_stream_close_callback close_callback;
	bk_stream_seek_callback seek_callback;
	bk_stream_read_callback read_callback;
	bk_stream_write_callback write_callback;
	bk_stream_eof_callback eof_callback;
} bk_stream_interface;

static bk_stream_interface* file_interface = BK_NULL;
static bk_stream_interface* data_interface = BK_NULL;
static bk_stream_interface* external_data_interface = BK_NULL;

#ifdef OS_WINDOWS
static bk_stream_interface* win32file_interface = BK_NULL;
#endif

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

static bk_bool bk_file_eof(void* userData)
{
	return feof((FILE*)userData);
}
//-------标准文件操作-------

//-------内存操作-------
static void bk_stream_buffer_close(void* userData)
{
	bk_buffer_destroy((bk_buffer*)userData);
}

static unsigned long long bk_stream_buffer_seek(void* userData, long long offset, int _where)
{
	bk_buffer* data = (bk_buffer*)userData;
	bk_buffer_seek(data, offset, _where);
	return bk_buffer_tell(data);
}

static size_t bk_stream_buffer_read(void* userData, void* readBuffer, size_t readSize)
{
	return bk_buffer_read((bk_buffer*)userData, readBuffer, readSize);
}

static size_t bk_stream_buffer_write(void* userData, const void* writeBuffer, size_t writeSize)
{
	bk_buffer_write((bk_buffer*)userData, writeBuffer, writeSize);
	return writeSize;
}

static bk_bool bk_stream_buffer_eof(void* userData)
{
	return bk_buffer_eof((bk_buffer*)userData);
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

static bk_bool bk_win32file_eof(void* userData)
{
	LARGE_INTEGER pos;
	LARGE_INTEGER offset;
	offset.QuadPart = pos.QuadPart = 0;
	SetFilePointerEx(userData, offset, &pos, FILE_CURRENT);
	GetFileSizeEx(userData, &offset);
	return offset.QuadPart == pos.QuadPart;
}
#endif
//-------win32文件操作-------


bk_stream_interface* bk_stream_interface_create(
	bk_stream_close_callback closeCallback,
	bk_stream_seek_callback seekCallback,
	bk_stream_read_callback readCallback,
	bk_stream_write_callback writeCallback,
	bk_stream_eof_callback eof_callback)
{
	bk_stream_interface* _interface = (bk_stream_interface*)malloc(sizeof(bk_stream_interface));
	_interface->stored = 0;
	_interface->ref = 0;
	_interface->close_callback = closeCallback;
	_interface->seek_callback = seekCallback;
	_interface->read_callback = readCallback;
	_interface->write_callback = writeCallback;
	_interface->eof_callback = eof_callback;
	return _interface;
}

void bk_stream_interface_destroy(bk_stream_interface* _interface)
{
	if (_interface->ref == 0 || --_interface->ref == 0)
	{
		if (_interface->stored)
			*_interface->stored = 0;
		free(_interface);
	}
}

void bk_stream_init(
	bk_stream* stream,
	void* userData,
	bk_stream_interface* _interface)
{
	stream->user_data = userData;
	stream->stream_interface = _interface;
	++_interface->ref;
}

static bk_stream_interface* bk_stream_interface_get_file()
{
	if (file_interface)
		return file_interface;
	file_interface = bk_stream_interface_create(bk_file_close, bk_file_seek, bk_file_read, bk_file_write, bk_file_eof);
	file_interface->stored = &file_interface;
	return file_interface;
}

static bk_stream_interface* bk_stream_interface_get_data()
{
	if (data_interface)
		return data_interface;
	data_interface = bk_stream_interface_create(bk_stream_buffer_close, bk_stream_buffer_seek, bk_stream_buffer_read, bk_stream_buffer_write, bk_stream_buffer_eof);
	data_interface->stored = &data_interface;
	return data_interface;
}

static bk_stream_interface* bk_stream_interface_get_external_data()
{
	if (external_data_interface)
		return external_data_interface;
	external_data_interface = bk_stream_interface_create(0, bk_stream_buffer_seek, bk_stream_buffer_read, bk_stream_buffer_write, bk_stream_buffer_eof);
	external_data_interface->stored = &external_data_interface;
	return external_data_interface;
}

#ifdef OS_WINDOWS
static bk_stream_interface* bk_stream_interface_get_win32file()
{
	if (win32file_interface)
		return win32file_interface;
	win32file_interface = bk_stream_interface_create(bk_win32file_close, bk_win32file_seek, bk_win32file_read, bk_win32file_write, bk_win32file_eof);
	win32file_interface->stored = &win32file_interface;
	return win32file_interface;
}
#endif

int bk_stream_init_with_file(bk_stream* stream, FILE* file)
{
	if (!file)
		return EINVAL;
	bk_stream_init(stream, file, bk_stream_interface_get_file());
	return BK_ERROR_OK;
}

int bk_stream_init_with_filename(bk_stream* stream, const char* filename, const char* mode)
{
	FILE* f = fopen(filename, mode);
	if (!f)
		return errno;

	int result = bk_stream_init_with_file(stream, f);
	if(result != BK_ERROR_OK)
		fclose(f);
	return result;
}

int bk_stream_init_with_buffer(bk_stream* stream, void* data, size_t size, bk_buffer_dtor dtor)
{
	if (!data && dtor != BK_BUFFER_COPY)
		return EINVAL;
	bk_buffer* dataUserData;

	if (dtor == BK_BUFFER_COPY && !data)
		dataUserData = bk_buffer_create(size);
	else
		dataUserData = bk_buffer_create_with_data(data, size, dtor);
	bk_buffer_limit(dataUserData, size);

	bk_stream_init(
		stream, 
		dataUserData,
		bk_stream_interface_get_data());
	return BK_ERROR_OK;
}

int bk_stream_init_with_external_buffer(bk_stream* stream, bk_buffer* data)
{
	if (!data)
		return EINVAL;
	bk_stream_init(
		stream,
		data,
		bk_stream_interface_get_external_data());
	return BK_ERROR_OK;
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
		bk_stream_interface_get_win32file());
	return BK_ERROR_OK;
}

void bk_stream_close(bk_stream* stream)
{
	if (stream->stream_interface->close_callback)
		stream->stream_interface->close_callback(stream->user_data);
	bk_stream_interface_destroy(stream->stream_interface);
}

unsigned long long bk_stream_seek(bk_stream* stream, long long offset, int _where)
{
	return stream->stream_interface->seek_callback(stream->user_data, offset, _where);
}

size_t bk_stream_read(bk_stream* stream, void* readBuffer, size_t readSize)
{
	return stream->stream_interface->read_callback(stream->user_data, readBuffer, readSize);
}

size_t bk_stream_write(bk_stream* stream, const void* data, size_t writeSize)
{
	return stream->stream_interface->write_callback(stream->user_data, data, writeSize);
}

bk_buffer* bk_stream_get_buffer(bk_stream* stream)
{
	return (bk_buffer*)stream->user_data;
}

bk_bool bk_stream_eof(bk_stream* stream)
{
	return stream->stream_interface->eof_callback(stream->user_data);
}

#endif
