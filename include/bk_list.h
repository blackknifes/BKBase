#ifndef __BK_LIST_H__
#define __BK_LIST_H__

#include "bk_export.h"
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
	struct bk_list_struct;
	typedef struct bk_list_struct bk_list;

	BK_EXPORT bk_list* bk_list_create(size_t valSize, void(*dtor)(void*));
	BK_EXPORT void bk_list_destroy(bk_list* _list);

	BK_EXPORT void bk_list_clear(bk_list* _list);
	BK_EXPORT void* bk_list_next(bk_list* _list, void* data);
	BK_EXPORT void* bk_list_pre(bk_list* _list, void* data);
	BK_EXPORT void* bk_list_begin(bk_list* _list);
	BK_EXPORT void* bk_list_end(bk_list* _list);

	BK_EXPORT size_t bk_list_get_size(bk_list* _list);
	BK_EXPORT void* bk_list_get_front(bk_list* _list);
	BK_EXPORT void* bk_list_get_back(bk_list* _list);

	BK_EXPORT void bk_list_push_front(bk_list* _list, const void* _data);
	BK_EXPORT void bk_list_push_back(bk_list* _list, const void* _data);
	BK_EXPORT void bk_list_pop_back(bk_list* _list);
	BK_EXPORT void bk_list_pop_front(bk_list* _list);
	BK_EXPORT void bk_list_insert(bk_list* _list, const void* inserted, const void* obj);
	BK_EXPORT void bk_list_insert_before(bk_list* _list, const void* inserted, const void* obj);
	BK_EXPORT void bk_list_insert_after(bk_list* _list, const void* inserted, const void* obj);
	BK_EXPORT void* bk_list_erase(bk_list* _list, void* data);
#ifdef __cplusplus
};
#endif
#endif