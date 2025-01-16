#pragma once

#include "DataStructPch.h"
#include "LinearQueue.h"

// Key의 길이가 unsigned인 PointerTable
class PointerTable32
{
public:
	PointerTable32() = default;
	~PointerTable32() = default;

	bool Initiate(UINT32 capacity);
	bool Terminate();

	bool Insert(UINT32 key, void* ptr);

	// Warning: This method does not release allocated memory. 
	bool Remove(UINT32 key);
	void* Get(UINT32 key) const;
	void* Pop(UINT32 key);

	UINT32 GetCount() const;

	void GetIdsTo(UINT32* out_keys, UINT32* out_numKeys) const;

	UINT32 GetCapacity() const;

private:
	UINT32 _capacity = 0;
	void** _entries = nullptr;
	UINT32 _count = 0;

	LinearQueue _usingKeyQueue;
};

// Key의 길이가 unsigned short인 PointerTable
class PointerTable16
{
public:
	PointerTable16() = default;
	~PointerTable16() = default;

	bool Initiate(UINT16 capacity);
	bool Terminate();

	bool Insert(UINT16 key, void* ptr);

	// Warning: This method does not release allocated memory. 
	bool Remove(UINT16 key);
	void* Get(UINT16 key) const;
	void* Pop(UINT16 key);

	UINT16 GetCount() const;

	void GetIdsTo(UINT16* out_keys, UINT16* out_numKeys) const;

	UINT16 GetCapacity() const;

private:
	UINT16 _capacity = 0;
	void** _entries = nullptr;
	UINT16 _count = 0;

	LinearQueue _usingKeyQueue;
};
