#include "file_test_util.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

char* bk_test_read_file(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	size_t fileSize = (size_t)ftell(f);
	rewind(f);
	char* buf = (char*)malloc(fileSize + 1);
	fread(buf, 1, fileSize, f);
	fclose(f);
	buf[fileSize] = 0;
	return buf;
}

void bk_test_read_file_free(char* str)
{
	free(str);
}

void bk_test_delete_file(const char* filename)
{
	unlink(filename);
}
