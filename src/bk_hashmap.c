﻿#include "bk_hashmap.h"
#include <malloc.h>
#include <memory.h>
#include <string.h>

#if defined(_WIN64)
#define BK_HASH_BASIS 14695981039346656037ULL
#define BK_HASH_PRIME 1099511628211ULL
#else
#define BK_HASH_BASIS 2166136261U
#define BK_HASH_PRIME 16777619U
#endif

typedef struct bk_hashmap_node_struct
{
	struct bk_hashmap_node_struct* next;
	void* key;
	void* value;
} bk_hashmap_node;

typedef struct bk_hashmap_struct 
{
	bk_hasher hasher;			//哈希函数
	bk_comparer comparer;		//比较函数
	bk_dtor key_dtor;			//键析构
	bk_dtor value_dtor;			//值析构

	size_t size;				//元素个数

	bk_hashmap_node** buckets;	//链桶
	size_t bucket_count;		//链桶个数
} bk_hashmap;

static size_t bk_hashmap_hashcode_with_basis(const void* _data, size_t _size, size_t basis)
{
	const unsigned char* ptr = (const unsigned char*)_data;
	for (size_t i = 0; i < _size; ++i)
	{
		basis ^= ptr[i];
		basis *= BK_HASH_PRIME;
	}
	return basis;
}

static void bk_hashmap_node_destroy(bk_hashmap_node* deletedNode, bk_dtor keyDtor, bk_dtor valDtor)
{
	if (keyDtor)
		keyDtor(deletedNode->key);
	if (valDtor)
		valDtor(deletedNode->value);
	free(deletedNode);
}

static size_t bk_hashmap_hashcode(const void* _data, size_t _size)
{
	return bk_hashmap_hashcode_with_basis(_data, _size, BK_HASH_BASIS);
}

static size_t bk_hashmap_default_hasher(const void* _data)
{
	return bk_hashmap_hashcode(&_data, sizeof(_data));
}

static size_t bk_hashmap_default_str_hasher(const void* _data)
{
	size_t len = strlen((const char*)_data);
	return bk_hashmap_hashcode(_data, len);
}

static bk_bool bk_hashmap_default_str_compare(const void* left, const void* right)
{
	const char* leftStr = (const char*)left;
	const char* rightStr = (const char*)right;
	return strcmp(leftStr, rightStr) == 0;
}

static bk_bool bk_hashmap_default_comparer(const void* left, const void* right)
{
	return left == right;
}

static void bk_hashmap_bucket_destroy(bk_hashmap_node* bucket, bk_dtor keyDtor, bk_dtor valDtor)
{
	bk_hashmap_node* node = bucket;
	while (node)
	{
		bk_hashmap_node* deletedNode = node;
		node = node->next;
		bk_hashmap_node_destroy(deletedNode, keyDtor, valDtor);
	}
}

bk_hashmap* bk_hashmap_create(bk_dtor keyDtor, bk_dtor valDtor)
{
	return bk_hashmap_create_with_interface(bk_hashmap_default_hasher, bk_hashmap_default_comparer, keyDtor, valDtor);
}

bk_hashmap* bk_hashmap_create_with_strkey(bk_dtor keyDtor, bk_dtor valDtor)
{
	return bk_hashmap_create_with_interface(bk_hashmap_default_str_hasher, bk_hashmap_default_str_compare, keyDtor, valDtor);
}

bk_hashmap* bk_hashmap_create_with_interface(bk_hasher hasher, bk_comparer comparer, bk_dtor keyDtor, bk_dtor valDtor)
{
	bk_hashmap* _map = (bk_hashmap*)malloc(sizeof(bk_hashmap));
	_map->hasher = hasher;
	_map->comparer = comparer;
	_map->key_dtor = keyDtor;
	_map->value_dtor = valDtor;
	_map->size = 0;
	_map->buckets = BK_NULL;
	_map->bucket_count = 0;
	return _map;
}

void bk_hashmap_destroy(bk_hashmap* _map)
{
	for (size_t i = 0; i < _map->bucket_count; ++i)
		bk_hashmap_bucket_destroy(_map->buckets[i], _map->key_dtor, _map->value_dtor);
	free(_map->buckets);
	free(_map);
}