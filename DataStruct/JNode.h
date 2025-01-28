#pragma once

#include "DataStructPch.h"

class JMap;

// Key의 크기는 32비트 고정
class JNode
{
	friend JMap;
public:
	void* GetPtr() { return _ptr; }
	UINT32 GetKey() { return _key; }
	JNode* Next() { return _next; }
	JNode* Prev() { return _prev; }

	void Reset() { _key = MAXUINT32; _ptr = nullptr; _prev = nullptr; _next = nullptr; }
	void SwapData(JNode* pOther) 
	{ 
		void* p = _ptr; 
		UINT32 k = _key;

		_ptr = pOther->_ptr;
		_key = pOther->_key;

		pOther->_ptr = p;
		pOther->_key = k;
	}
private:
	UINT32 _key = MAXUINT32;
	void* _ptr = nullptr;
	JNode* _prev = nullptr;
	JNode* _next = nullptr;
};