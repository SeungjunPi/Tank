#pragma once

#include "stdafx.h"

#define NET_STREAM_DEFAULT_SIZE (65536)

// 
class Stream
{
public:
	Stream();
	Stream(size_t size);
	~Stream();

	BYTE* GetReadPtr() const;
	BYTE* GetWritePtr() const;

	size_t GetDataSize() const;
	size_t GetFreeSize() const;

	bool ShiftReadPtr(size_t len);
	bool ShiftWritePtr(size_t len);

	void Rearrangement();

	void Flush();



private:
	const size_t _capacity;

	BYTE* const _pBegin;
	BYTE* const _pEnd;

	BYTE* _head;
	BYTE* _tail;

};

