#include "PointerMap.h"

PointerMap::PointerMap(int(*hash)(void* ptr), int capacity)
	: _capacity(capacity)
	, _FuncHash(hash)
	, _count(0)
	, _minValidIndex(capacity)
{
	_entries = (void**)malloc(sizeof(void*) * capacity);
	assert(_entries != NULL);
	ZeroMemory(_entries, sizeof(void*) * capacity);
}

PointerMap::~PointerMap()
{
	free(_entries);
}

int PointerMap::GetHash(void* ptr)
{
	return _FuncHash(ptr);
}

bool PointerMap::Insert(void* ptr)
{
	int index = _FuncHash(ptr);

	if (_entries[index] != NULL) {
		return false;
	}

	_entries[index] = ptr;
	++_count;

	if (_minValidIndex == _capacity) {
		_minValidIndex = index;
	}
	else if (index < _minValidIndex) {
		_minValidIndex = index;
	}

	return true;
}

void PointerMap::Remove(int key)
{
	assert(key < _capacity);
	_entries[key] = NULL;
	--_count;
	UpdateValidIndex();
}

void* PointerMap::Get(int key)
{
	assert(key < _capacity);
	return _entries[key];
}

void* PointerMap::Pop(int key)
{
	assert(key < _capacity);
	void* ptr = _entries[key];
	_entries[key] = NULL;
	--_count;
	UpdateValidIndex();
	return ptr;
}

void* PointerMap::PopFront()
{
	if (_minValidIndex == _capacity) {
		return NULL;
	}

	void* ptr = _entries[_minValidIndex];
	_entries[_minValidIndex] = NULL;
	--_count;
	UpdateValidIndex();
	return ptr;
}

int PointerMap::GetCount() const
{
	return _count;
}

void PointerMap::UpdateValidIndex()
{
	while (_minValidIndex < _capacity) {
		if (_entries[_minValidIndex] != NULL) {
			break;
		}
		++_minValidIndex;
	}
}

