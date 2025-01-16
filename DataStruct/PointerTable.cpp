#include "PointerTable.h"
#include "LinearQueue.h"

bool PointerTable32::Initiate(UINT32 capacity)
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

bool PointerTable32::Insert(UINT32 key, void* ptr)
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

bool PointerTable32::Remove(UINT32 key)
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

void* PointerTable32::Get(UINT32 key) const
{
	return _entries[key];
}

void* PointerTable32::Pop(UINT32 key)
{
	void* tmp = _entries[key];

	if (tmp != nullptr) {
		_entries[key] = nullptr;
		--_count;
		_usingKeyQueue.Remove(&key);
	}
	return tmp;
}

UINT32 PointerTable32::GetCount() const
{
	return _count;
}

void PointerTable32::GetIdsTo(UINT32* out_keys, UINT32* out_numKeys) const
{
	*out_numKeys = _usingKeyQueue.GetCount();
	UINT32* keys = (UINT32*)_usingKeyQueue.GetHead();
	memcpy(out_keys, keys, sizeof(UINT32) * (*out_numKeys));
}

UINT32 PointerTable32::GetCapacity() const
{
	return _capacity;
}




bool PointerTable16::Initiate(UINT16 capacity)
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

bool PointerTable16::Insert(UINT16 key, void* ptr)
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

bool PointerTable16::Remove(UINT16 key)
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

void* PointerTable16::Get(UINT16 key) const
{
	return _entries[key];
}

void* PointerTable16::Pop(UINT16 key)
{
	void* tmp = _entries[key];

	if (tmp != nullptr) {
		_entries[key] = nullptr;
		--_count;
		_usingKeyQueue.Remove(&key);
	}
	return tmp;
}

UINT16 PointerTable16::GetCount() const
{
	return _count;
}

void PointerTable16::GetIdsTo(UINT16* out_keys, UINT16* out_numKeys) const
{
	*out_numKeys = _usingKeyQueue.GetCount();
	UINT16* keys = (UINT16*)_usingKeyQueue.GetHead();
	memcpy(out_keys, keys, sizeof(UINT16) * (*out_numKeys));
}

UINT16 PointerTable16::GetCapacity() const
{
	return _capacity;
}

