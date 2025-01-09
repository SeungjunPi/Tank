#pragma once

#include "DataStructPch.h"

struct PointerNode
{
	PointerNode* prev = nullptr;
	PointerNode* next = nullptr;
	void* pData = nullptr;
};


class PointerList
{
public:
	void Initiate(int capacity);
	void Terminate();
	void Add(void* pData);
	void Remove(void* pData);

	int GetCount() const;
	BOOL IsEmpty() const;
private:
	int _capacity = 0;
	int _count = 0;

	PointerNode* _begin = nullptr;
	PointerNode* _end = nullptr;

	PointerNode* _head = nullptr;
	PointerNode* _tail = nullptr;

	PointerNode* FindEmptyNode();
};

