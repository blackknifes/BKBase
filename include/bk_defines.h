#ifndef __BK_DEFINES_H__
#define __BK_DEFINES_H__


#define BK_ALIGN(size) (((size) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))
#endif