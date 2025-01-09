#include "PointerList.h"

void PointerList::Initiate(int capacity)
{
	_begin = new PointerNode[capacity];
	_end = _begin + capacity;
	_head = _begin;
	_tail = nullptr;
	_capacity = capacity;
}

void PointerList::Terminate()
{
	delete[] _begin;
}

void PointerList::Add(void* pData)
{
	if (_count == _capacity) {
		__debugbreak();
	}

	PointerNode* pNode = FindEmptyNode();
	pNode->pData = pData;
	_count++;
	if (_count == 1) {
		_head = pNode;
		_tail = pNode;
		return;
	}

	_tail->next = pNode;
	pNode->prev = _tail;
	_tail = pNode;
}

void PointerList::Remove(void* pData)
{
	if (_count == 0) {
		__debugbreak();
		return;
	}

	PointerNode* pNode = _head;
	for (int i = 0; i < _count; ++i) {
		if (pNode->pData == pData) {
			break;
		}
		pNode = pNode->next;
	}

	if (pNode == nullptr) {
		// No matching data
		return;
	}

	PointerNode* pPrevNode = pNode->prev;
	PointerNode* pNextNode = pNode->next;
	memset(pNode, 0, sizeof(PointerNode));

	if (_count == 1) {		
		assert(_head == _tail);
		_head = _begin;
		_tail = nullptr;
		_count--;
		return;
	}

	assert(_head != _tail);

	if (pNode == _head) {
		assert(pNode->prev == nullptr);
		pNextNode->prev = nullptr;
		_head = pNextNode;
	}
	else if (pNode == _tail) {
		assert(pNode->next == nullptr);
		pPrevNode->next = nullptr;
		_tail = pPrevNode;
	}
	else {
		pPrevNode->next = pNextNode;
		pNextNode->prev = pPrevNode;
	}
	_count--;
}

int PointerList::GetCount() const
{
	return _count;
}

BOOL PointerList::IsEmpty() const
{
	return _count == 0;
}

PointerNode* PointerList::FindEmptyNode()
{
	for (PointerNode* pNode = _begin; pNode < _end; ++pNode) {
		if (pNode->pData == nullptr) {
			return pNode;
		}
	}

	__debugbreak();

	return nullptr;
}

