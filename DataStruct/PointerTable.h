#pragma once

#include "DataStructPch.h"
#include "LinearQueue.h"

// Key의 길이가 unsigned인 PointerTable
class PointerTable32
{
public:
	PointerTable32() = default;
	~PointerTable32() = default;

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

// Key의 길이가 unsigned short인 PointerTable
class PointerTable16
{
public:
	PointerTable16() = default;
	~PointerTable16() = default;

	bool Initiate(int capacity);
	bool Terminate();

	bool Insert(unsigned short key, void* ptr);

	// Warning: This method does not release allocated memory. 
	bool Remove(unsigned short key);
	void* Get(unsigned short key) const;
	void* Pop(unsigned short key);

	int GetCount() const;

	void GetIdsTo(unsigned short* out_keys, int* out_numKeys) const;

	int GetCapacity() const;

private:
	int _capacity = 0;
	void** _entries = nullptr;
	int _count = 0;

	LinearQueue _usingKeyQueue;
};
