#include "JStack.h"

JStack::JStack(UINT32 entrySize, UINT32 maxCount)
	: _entrySize(entrySize)
	, _maxCount(maxCount)
{
	_data = DNew CHAR[_entrySize * _maxCount];
}

JStack::~JStack()
{
	delete[] _data;
}

void JStack::Push(const void* src)
{
	memcpy(_data + _entrySize * _top, src, _entrySize);
	++_top;
}

void JStack::Top(void* dst) const
{
	if (IsEmpty()) {
		__debugbreak();
		return;
	}

	memcpy(dst, _data + _entrySize * _top, _entrySize);
}

void JStack::Pop()
{
	if (IsEmpty()) {
		__debugbreak();
		return;
	}
	--_top;
}

void JStack::Clear()
{
	_top = 0;
}






