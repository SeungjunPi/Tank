#include "PointerQueue.h"

#include "stdafx.h"

PointerQueue::PointerQueue(int capacity)
	: _capacity(capacity)
	, _count(0)
	, _head(0)
	, _tail(0)
{
	_entries = (void**)malloc(sizeof(void*) * capacity);
}

PointerQueue::~PointerQueue()
{
	free(_entries);
}

bool PointerQueue::Enqueue(void* entry)
{
	if (_count == _capacity) {
		return false;
	}

	if (_tail == _capacity && _head != 0) {
		Rearrangement();
	}

	_entries[_tail++] = entry;
	++_count;
	return true;
}

void* PointerQueue::Dequeue()
{
	if (_head == _tail) {
		return NULL;
	}

	--_count;
	void* ptr = _entries[_head];
	++_head;
	return ptr;
}

int PointerQueue::GetCount() const
{
	return _count;
}

void PointerQueue::Rearrangement()
{
	memmove(_entries, _entries + _head, _count * sizeof(void*));
	_head = 0;
	_tail = _count;
}




