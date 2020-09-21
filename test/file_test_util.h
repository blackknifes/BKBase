#ifndef __FILE_TEST_UTIL_H__
#define __FILE_TEST_UTIL_H__

char* bk_test_read_file(const char* filename);
void bk_test_read_file_free(char* str);
void bk_test_delete_file(const char* filename);
#endif