#include "bk_list.h"
#include <assert.h>
#include <malloc.h>
#include <memory.h>

#define ALIGN(size) (((size) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))
#define BK_LIST_GET_NODE(ptr) ((struct bk_list_node_struct*)((unsigned char*)ptr - (sizeof(struct bk_list_node_struct) << 1)))

typedef struct bk_list_node_struct
{
	struct bk_list_node_struct* pre;
	struct bk_list_node_struct* next;
	unsigned char data[1];						//存储数据
} bk_list_node;

typedef struct bk_list_struct 
{
	bk_list_node* front;
	bk_list_node* back;
	size_t value_size;
	size_t size;
	void(*dtor)(void*);
} bk_list;

static bk_list_node* bk_list_node_create(size_t valSize)
{
	bk_list_node* node = (bk_list_node*)malloc(valSize + (sizeof(struct bk_list_struct*) << 1));
	node->pre = node->next = 0;
	return node;
}

static bk_list_node* bk_list_node_create_with_data(size_t valSize, const void* _data)
{
	bk_list_node* node = bk_list_node_create(valSize);
	memcpy(node->data, _data, valSize);
	return node;
}

static void bk_list_node_destroy(bk_list_node* node, void(*dtor)(void*))
{
	if (dtor)
		dtor(node->data);
	free(node);
}

bk_list* bk_list_create(size_t valSize, void(*dtor)(void*))
{
	assert(valSize != 0);
	bk_list* _list = (bk_list*)malloc(sizeof(bk_list));
	_list->front = _list->back = 0;
	_list->value_size = ALIGN(valSize);
	_list->size = 0;
	_list->dtor = dtor;
	return _list;
}

void bk_list_destroy(bk_list* _list)
{
	bk_list_clear(_list);
	free(_list);
}

void bk_list_clear(bk_list* _list)
{
	bk_list_node* node = _list->front;
	if (_list->dtor)
	{
		while (node)
		{
			_list->dtor(node->data);
			bk_list_node* preNode = node;
			node = node->next;
			free(preNode);
		}
	}
	else
	{
		while (node)
		{
			bk_list_node* preNode = node;
			node = node->next;
			free(preNode);
		}
	}
	_list->size = 0;
	_list->front = _list->back = 0;
}

void* bk_list_get_next(void* data)
{
	bk_list_node* node = BK_LIST_GET_NODE(data)->next;
	if (!node)
		return 0;
	return node->data;
}

void* bk_list_get_pre(void* data)
{
	bk_list_node* node = BK_LIST_GET_NODE(data)->pre;
	if (!node)
		return 0;
	return node->data;
}

size_t bk_list_get_size(bk_list* _list)
{
	return _list->size;
}

void* bk_list_get_front(bk_list* _list)
{
	assert(_list->front);
	return _list->front->data;
}

void* bk_list_get_back(bk_list* _list)
{
	assert(_list->back);
	return _list->back->data;
}

void bk_list_push_front(bk_list* _list, const void* _data)
{
	bk_list_node* node = bk_list_node_create_with_data(_list->value_size, _data);
	node->next = _list->front;
	if(_list->front)
		_list->front->pre = node;
	_list->front = node;
	++_list->size;
}

void bk_list_push_back(bk_list* _list, const void* _data)
{
	bk_list_node* node = bk_list_node_create_with_data(_list->value_size, _data);
	node->pre = _list->back;
	if(_list->back)
		_list->back->next = node;
	_list->back = node;
	++_list->size;
}

void bk_list_pop_back(bk_list* _list)
{
	assert(_list->back);
	bk_list_node* node = _list->back->pre;
	bk_list_node_destroy(_list->back, _list->dtor);
	if (node)
		node->next = 0;
	_list->back = node;
	--_list->size;
}

void bk_list_pop_front(bk_list* _list)
{
	assert(_list->front);
	bk_list_node* node = _list->front->next;
	bk_list_node_destroy(_list->front, _list->dtor);
	if (node)
		node->pre = 0;
	_list->front = node;
	--_list->size;
}

void bk_list_insert(bk_list* _list, const void* inserted, const void* obj)
{
	bk_list_insert_before(_list, inserted, obj);
}

void bk_list_insert_before(bk_list* _list, const void* inserted, const void* obj)
{
	if (!obj)
	{
		bk_list_push_back(_list, inserted);
		return;
	}
	bk_list_node* node = bk_list_node_create_with_data(_list->value_size, inserted);
	bk_list_node* objNode = BK_LIST_GET_NODE(obj);
	bk_list_node* pre = objNode->pre;
	if (pre)
		pre->next = node;
	else
		_list->front = node;
	node->pre = pre;
	node->next = objNode;
	objNode->pre = node;
	++_list->size;
}

void bk_list_insert_after(bk_list* _list, const void* inserted, const void* obj)
{
	if (!obj)
	{
		bk_list_push_front(_list, inserted);
		return;
	}
	bk_list_node* node = bk_list_node_create_with_data(_list->value_size, inserted);
	bk_list_node* objNode = BK_LIST_GET_NODE(obj);
	bk_list_node* next = objNode->next;
	if (next)
		next->pre = node;
	else
		_list->back = node;
	node->next = next;
	node->pre = objNode;
	objNode->next = node;
	++_list->size;
}

void bk_list_erase(bk_list* _list, void* data)
{
	bk_list_node* node = BK_LIST_GET_NODE(data);
	bk_list_node* pre = node->pre;
	bk_list_node* next = node->next;
	if (pre)
		pre->next = next;
	else
		_list->front = next;
	if (next)
		next->pre = pre;
	else
		_list->back = pre;

	--_list->size;
	bk_list_node_destroy(node, _list->dtor);
}
