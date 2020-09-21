#ifndef __BK_TYPES_H__
#define __BK_TYPES_H__
#include <stdint.h>
#include <intsafe.h>

#define BK_NULL 0
#define BK_TRUE 1
#define BK_FALSE 0

typedef int bk_bool;
typedef unsigned char bk_byte;
typedef char bk_char;
typedef unsigned short bk_char16;

typedef short bk_int16;
typedef unsigned short bk_uint16;
typedef int bk_int32;
typedef unsigned int bk_uint32;
typedef long bk_long;
typedef unsigned long bk_ulong;
typedef long long bk_int64;
typedef unsigned long long bk_uint64;

typedef float bk_float;
typedef double bk_double;
typedef float bk_real;


typedef char* bk_cstr;
typedef const char* bk_const_cstr;


typedef void* bk_potiner;
typedef const void* bk_const_potiner;

typedef size_t bk_size_t;

#ifdef ARCH_X64
typedef long long bk_ssize_t;
#else
typedef int bk_ssize_t;
#endif

#endif