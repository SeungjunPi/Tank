#pragma once

#include "stdafx.h"

class PointerMap
{
public:
	PointerMap(int (*hash)(void* ptr), int capacity);
	virtual ~PointerMap();

	int GetHash(void* ptr);
	bool Insert(void* ptr);
	void Remove(int key);
	void* Get(int key);
	void* Pop(int key);
	void* PopFront();

	int GetCount() const;

private:
	const int _capacity;
	int (*_FuncHash)(void* ptr);
	void** _entries;
	int _count;

	int _minValidIndex;

	
	void UpdateValidIndex();
};
