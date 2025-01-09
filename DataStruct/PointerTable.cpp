#include "PointerTable.h"
#include "LinearQueue.h"

bool PointerTable32::Initiate(int capacity)
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

bool PointerTable32::Terminate()
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

bool PointerTable32::Insert(int key, void* ptr)
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

bool PointerTable32::Remove(int key)
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

void* PointerTable32::Get(int key) const
{
	return _entries[key];
}

void* PointerTable32::Pop(int key)
{
	void* tmp = _entries[key];

	if (tmp != nullptr) {
		_entries[key] = nullptr;
		--_count;
		_usingKeyQueue.Remove(&key);
	}
	return tmp;
}

int PointerTable32::GetCount() const
{
	return _count;
}

void PointerTable32::GetIdsTo(int* out_keys, int* out_numKeys) const
{
	*out_numKeys = _usingKeyQueue.GetCount();
	int* keys = (int*)_usingKeyQueue.GetHead();
	memcpy(out_keys, keys, sizeof(int) * (*out_numKeys));
}

int PointerTable32::GetCapacity() const
{
	return _capacity;
}




bool PointerTable16::Initiate(int capacity)
{
	_capacity = capacity;
	_count = 0;
	_entries = (void**)malloc(sizeof(void*) * _capacity);
	if (_entries == nullptr) {
		// Handle System Error
		return false;
	}

	_usingKeyQueue.Initiate(sizeof(unsigned short), _capacity);

	ZeroMemory(_entries, sizeof(void*) * _capacity);

	return true;
}

bool PointerTable16::Terminate()
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

bool PointerTable16::Insert(unsigned short key, void* ptr)
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

bool PointerTable16::Remove(unsigned short key)
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

void* PointerTable16::Get(unsigned short key) const
{
	return _entries[key];
}

void* PointerTable16::Pop(unsigned short key)
{
	void* tmp = _entries[key];

	if (tmp != nullptr) {
		_entries[key] = nullptr;
		--_count;
		_usingKeyQueue.Remove(&key);
	}
	return tmp;
}

int PointerTable16::GetCount() const
{
	return _count;
}

void PointerTable16::GetIdsTo(unsigned short* out_keys, int* out_numKeys) const
{
	*out_numKeys = _usingKeyQueue.GetCount();
	int* keys = (int*)_usingKeyQueue.GetHead();
	memcpy(out_keys, keys, sizeof(int) * (*out_numKeys));
}

int PointerTable16::GetCapacity() const
{
	return _capacity;
}

