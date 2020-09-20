#ifndef __BK_VECTOR_H__
#define __BK_VECTOR_H__
#include "bk_export.h"
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
	struct bk_vector_struct;
	typedef struct bk_vector_struct bk_vector;

	BK_EXPORT bk_vector* bk_vector_create(size_t valSize, void(*dtor)(void*));
	BK_EXPORT void bk_vector_destroy(bk_vector* vec);

	BK_EXPORT void* bk_vector_get_array_pointer(bk_vector* vec);

	BK_EXPORT void bk_vector_reserve(bk_vector* vec, size_t capacity);
	BK_EXPORT void bk_vector_resize(bk_vector* vec, size_t size);
	BK_EXPORT size_t bk_vector_get_capcity(bk_vector* vec);
	BK_EXPORT size_t bk_vector_get_size(bk_vector* vec);
	BK_EXPORT void* bk_vector_get_data(bk_vector* vec, size_t index);
	BK_EXPORT void* bk_vector_get_front(bk_vector* vec);
	BK_EXPORT void* bk_vector_get_back(bk_vector* vec);

	BK_EXPORT void bk_vector_push_back(bk_vector* vec, const void* _data);
	BK_EXPORT void bk_vector_pop_back(bk_vector* vec);
	BK_EXPORT void bk_vector_insert(bk_vector* vec, const void* _data, size_t index);
	BK_EXPORT void bk_vector_erase(bk_vector* vec, size_t index);
#ifdef __cplusplus
};
#endif
#endif