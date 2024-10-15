#include "PointerTable.h"
#include "LinearQueue.h"

bool PointerTable::Initiate(int capacity)
{
	_capacity = capacity;
	_count = 0;
	_entries = (void**)malloc(sizeof(void*) * _capacity);
	if (_entries == nullptr) {
		// Handle System Error
		return false;
	}

	_usingKeyQueue.Initiate(sizeof(int), _capacity);

	ZeroMemory(_entries, sizeof(void*) * _capacity);

	return true;
}

bool PointerTable::Terminate()
{
	if (_count > 0 || _entries == nullptr) {
		__debugbreak();
		return false;
	}
	_usingKeyQueue.Terminate();
	free(_entries);
	_count = 0;
	_capacity = 0;
	return true;
}

bool PointerTable::Insert(int key, void* ptr)
{
	if (_entries[key] != nullptr) {
		__debugbreak();
		return false;
	}

	_usingKeyQueue.Push(&key);

	_entries[key] = ptr;
	++_count;
	return true;
}

bool PointerTable::Remove(int key)
{
	if (_entries[key] == nullptr) {
		__debugbreak();
		return false;
	}

	_usingKeyQueue.Remove(&key);

	_entries[key] = nullptr;
	--_count;
	return true;
}

void* PointerTable::Get(int key) const
{
	return _entries[key];
}

void* PointerTable::Pop(int key)
{
	void* tmp = _entries[key];

	if (tmp != nullptr) {
		_entries[key] = nullptr;
		--_count;
		_usingKeyQueue.Remove(&key);
	}
	return tmp;
}

int PointerTable::GetCount() const
{
	return _count;
}

void PointerTable::GetIdsTo(int* out_keys, int* out_numKeys) const
{
	*out_numKeys = _usingKeyQueue.GetCount();
	int* keys = (int*)_usingKeyQueue.GetHead();
	memcpy(out_keys, keys, sizeof(int) * (*out_numKeys));
}

int PointerTable::GetCapacity() const
{
	return _capacity;
}
