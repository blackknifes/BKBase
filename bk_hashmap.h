#ifndef __BK_HASHMAP_H__
#define __BK_HASHMAP_H__
#include "bk_export.h"
#include "bk_types.h"
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

	struct bk_hashmap_struct;
	typedef struct bk_hashmap_struct bk_hashmap;
	struct bk_hashmap_node_struct;
	typedef struct bk_hashmap_node_struct bk_hashmap_node;
	typedef struct bk_hashmap_iterator_struct
	{
		bk_hashmap* container;
		bk_hashmap_node** ppNext;
		bk_hashmap_node* node;
	}bk_hashmap_iterator;

	typedef size_t(*bk_hasher)(const void*);
	typedef bk_bool(*bk_comparer)(const void*, const void*);
	typedef void(*bk_dtor)(void*);

	BK_EXPORT bk_hashmap* bk_hashmap_create(bk_dtor keyDtor, bk_dtor valDtor);
	BK_EXPORT bk_hashmap* bk_hashmap_create_with_strkey(bk_dtor keyDtor, bk_dtor valDtor);
	BK_EXPORT bk_hashmap* bk_hashmap_create_with_interface(bk_hasher hasher, bk_comparer comparer, bk_dtor keyDtor, bk_dtor valDtor);
	BK_EXPORT void bk_hashmap_destroy(bk_hashmap* _map);
	BK_EXPORT void bk_hashmap_insert(bk_hashmap* _map, void* key, void* value);
	BK_EXPORT bk_bool bk_hashmap_erase(bk_hashmap* _map, const void* key);
	BK_EXPORT void bk_hashmap_clear(bk_hashmap* _map);
	BK_EXPORT void bk_hashmap_rehash(bk_hashmap* _map, size_t bucketCount);

	BK_EXPORT bk_hashmap_iterator bk_hashmap_find(bk_hashmap* _map, const void* key);

	BK_EXPORT bk_hashmap_iterator bk_hashmap_iterator_begin(bk_hashmap* _map);

	BK_EXPORT bk_bool bk_hashmap_iterator_is_valid(const bk_hashmap_iterator* itor);
	BK_EXPORT const void* bk_hashmap_iterator_get_key(const bk_hashmap_iterator* itor);
	BK_EXPORT void* bk_hashmap_iterator_get_value(const bk_hashmap_iterator* itor);
	BK_EXPORT void bk_hashmap_iterator_set_value(const bk_hashmap_iterator* itor, void* val);
	BK_EXPORT void bk_hashmap_iterator_next(bk_hashmap_iterator* itor);
	BK_EXPORT void bk_hashmap_iterator_remove(bk_hashmap_iterator* itor);
#ifdef __cplusplus
};
#endif
#endif