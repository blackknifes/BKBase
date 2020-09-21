#ifndef __BK_DATA_H__
#define __BK_DATA_H__
#include "bk_export.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
	struct bk_data_struct;
	typedef struct bk_data_struct bk_data;

	BK_EXPORT bk_data* bk_data_create(size_t size);
	BK_EXPORT bk_data* bk_data_create_with_sub(bk_data* data, size_t offset, size_t size);
	BK_EXPORT void bk_data_destroy(bk_data* data);

	BK_EXPORT size_t bk_data_get_size(const bk_data* data);
	BK_EXPORT const void* bk_data_get_data(const bk_data* data);

	BK_EXPORT size_t bk_data_write(bk_data* data, const void* buf, size_t offset, size_t size);
	BK_EXPORT size_t bk_data_read(const bk_data* data, void* buf, size_t offset, size_t size);
#ifdef __cplusplus
};
#endif
#endif