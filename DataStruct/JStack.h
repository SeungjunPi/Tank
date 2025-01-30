#pragma once

#include "DataStructPch.h"


// 고정된 크기의 스택
// 가득찰 경우, 중단점이 잡힘.
class JStack
{
public:
	JStack(UINT32 entrySize, UINT32 maxCount);
	~JStack();

	// src에서 entrySize만큼 복사
	void Push(const void* src);

	void Top(void* dst) const;
	void Pop();
	

	// 내용을 모두 무시하고, 초기화
	void Clear();

	bool IsEmpty() const { return _top == 0; }
	UINT32 GetCount() const { return _top; }

private:
	const UINT32 _entrySize;
	const UINT32 _maxCount;
	UINT32 _top = 0;
	
	CHAR* _data;
};


