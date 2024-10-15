#pragma once

#include "stdafx.h"

#include "LinearQueue.h"

class PointerTable
{
public:
	PointerTable() = default;
	~PointerTable() = default;

	bool Initiate(int capacity);
	bool Terminate();

	bool Insert(int key, void* ptr);

	// Warning: This method does not release allocated memory. 
	bool Remove(int key);
	void* Get(int key) const;
	void* Pop(int key);

	int GetCount() const;

	void GetIdsTo(int* out_keys, int* out_numKeys) const;

	int GetCapacity() const;

private:
	int _capacity = 0;
	void** _entries = nullptr;
	int _count = 0;

	LinearQueue _usingKeyQueue;
};
