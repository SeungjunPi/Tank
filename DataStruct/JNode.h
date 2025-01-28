#pragma once

#include "DataStructPch.h"

class JMap;

// Key의 크기는 32비트 고정
class JNode
{
	friend JMap;
public:
	void* GetPtr() { return _ptr; }
	JNode* Next() { return _next; }
	JNode* Prev() { return _prev; }
private:
	UINT32 _key = MAXUINT32;
	void* _ptr = nullptr;
	JNode* _prev = nullptr;
	JNode* _next = nullptr;
};