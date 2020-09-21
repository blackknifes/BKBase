#include "bk_hashmap.h"
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

void bk_hashmap_insert(bk_hashmap* _map, void* key, void* value)
{
	if(_map->bucket_count == 0)
	{
		_map->bucket_count = 24;
		_map->buckets = (bk_hashmap_node**)malloc(sizeof(bk_hashmap_node*) * _map->bucket_count);
		memset(_map->buckets, 0, sizeof(bk_hashmap_node*) * _map->bucket_count);
	}
	else if(_map->size / _map->bucket_count > 5)
		bk_hashmap_rehash(_map, _map->bucket_count << 2);

	size_t hashCode = _map->hasher(key);
	size_t bucketIndex = hashCode % _map->bucket_count;
	bk_hashmap_node** bucket = &_map->buckets[bucketIndex];
	while(*bucket && !_map->comparer((*bucket)->key, key))
		bucket = &(*bucket)->next;

	bk_hashmap_node* node = *bucket;

	if (node)
	{
		if (_map->key_dtor)
			_map->key_dtor(node->key);
		if (_map->value_dtor)
			_map->value_dtor(node->value);
	}
	else
	{
		node = *bucket = (bk_hashmap_node*)malloc(sizeof(bk_hashmap_node));
		node->next = BK_NULL;
		++_map->size;
	}
	node->key = key;
	node->value = value;
}

bk_bool bk_hashmap_erase(bk_hashmap* _map, const void* key)
{
	if (_map->size == 0)
		return BK_FALSE;
	size_t hashCode =_map->hasher(key);
	size_t index = hashCode % _map->bucket_count;
	bk_hashmap_node* node = _map->buckets[index];
	bk_hashmap_node** ppNode = &_map->buckets[index];
	while(node)
	{
		if(_map->comparer(node->key, key))
		{
			*ppNode = node->next;
			bk_hashmap_node_destroy(node, _map->key_dtor, _map->value_dtor);
			--_map->size;
			return BK_TRUE;
		}
		ppNode = &node->next;
		node = node->next;
	}
	return BK_FALSE;
}

void bk_hashmap_clear(bk_hashmap* _map)
{
	for (size_t i = 0; i < _map->bucket_count; ++i)
	{
		if (_map->buckets[i])
			bk_hashmap_bucket_destroy(_map->buckets[i], _map->key_dtor, _map->value_dtor);
	}
	memset(_map->buckets, 0, sizeof(bk_hashmap_node*) * _map->bucket_count);
	_map->size = 0;
}

void bk_hashmap_rehash(bk_hashmap* _map, size_t bucketCount)
{
	if (bucketCount < _map->bucket_count)
		return;
	bk_hashmap_node** buckets = (bk_hashmap_node**)malloc(sizeof(bk_hashmap_node*) * bucketCount);	
	memset(buckets, 0, sizeof(bk_hashmap_node*) * bucketCount);
	bk_hashmap_node*** lastNodes = (bk_hashmap_node***)malloc(sizeof(bk_hashmap_node**) * bucketCount);

	for (size_t i = 0; i < bucketCount; ++i)
		lastNodes[i] = &buckets[i];

	for (size_t i = 0; i < _map->bucket_count; ++i)
	{
		bk_hashmap_node* node = _map->buckets[i];
		while (node)
		{
			size_t hashCode = _map->hasher(node->key);
			size_t index = hashCode % bucketCount;
			bk_hashmap_node* curNode = node;
			node = node->next;

			*lastNodes[index] = curNode;
			curNode->next = BK_NULL;
			lastNodes[index] = &curNode->next;
		}
	}
	free(lastNodes);
	free(_map->buckets);

	_map->buckets = buckets;
	_map->bucket_count = bucketCount;
}

bk_hashmap_iterator bk_hashmap_find(bk_hashmap* _map, const void* key)
{
	bk_hashmap_iterator itor;
	itor.container = _map;
	itor.node = BK_NULL;
	itor.next = BK_NULL;
	itor.bucket_index = 0;
	if (_map->size == 0)
		return itor;
	size_t hashCode = _map->hasher(key);
	size_t index = hashCode % _map->bucket_count;
	bk_hashmap_node* node = _map->buckets[index];
	bk_hashmap_node** ppNode = &_map->buckets[index];
	while (node)
	{
		if(_map->comparer(node->key, key))
		{
			itor.node = node;
			itor.next = ppNode;
			itor.bucket_index = index;
			break;
		}

		ppNode = &node->next;
		node = node->next;
	}
	return itor;
}

bk_hashmap_iterator bk_hashmap_iterator_begin(bk_hashmap* _map)
{
	bk_hashmap_iterator itor;
	itor.container = _map;
	if(_map->size == 0)
	{
		itor.node = BK_NULL;
		itor.next = BK_NULL;
		itor.bucket_index = 0;
		return itor;
	}

	for (size_t i = 0; i < _map->bucket_count; ++i)
	{
		if(_map->buckets[i])
		{
			itor.node = _map->buckets[i];
			itor.next = &_map->buckets[i];
			itor.bucket_index = i;
			break;
		}
	}
	return itor;
}

bk_bool bk_hashmap_iterator_is_valid(const bk_hashmap_iterator* itor)
{
	return itor->node != BK_NULL;
}

const void* bk_hashmap_iterator_get_key(const bk_hashmap_iterator* itor)
{
	return itor->node->key;
}

void* bk_hashmap_iterator_get_value(const bk_hashmap_iterator* itor)
{
	return itor->node->value;
}

void bk_hashmap_iterator_set_value(const bk_hashmap_iterator* itor, void* val)
{
	if (itor->container->value_dtor)
		itor->container->value_dtor(itor->node->value);
	itor->node->value = val;
}

void bk_hashmap_iterator_next(bk_hashmap_iterator* itor)
{
	itor->next = &itor->node->next;
	itor->node = *itor->next;
	
	if(!itor->node)
	{
		for (size_t i = itor->bucket_index + 1; i < itor->container->bucket_count; ++i)
		{
			if (itor->container->buckets[i])
			{
				itor->node = itor->container->buckets[i];
				itor->next = &itor->container->buckets[i];
				itor->bucket_index = i;
				break;
			}
		}
	}
}

void bk_hashmap_iterator_remove(bk_hashmap_iterator* itor)
{
	bk_hashmap_node* deletedNode = itor->node;

	*itor->next = deletedNode->next;
	itor->node = deletedNode->next;
	--itor->container->size;
	bk_hashmap_node_destroy(deletedNode, itor->container->key_dtor, itor->container->value_dtor);
}
