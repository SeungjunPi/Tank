#include "Stream.h"

// Todo: 메모리 할당이 아닌 pool로 변경. 
Stream::Stream()
	: _capacity(NET_STREAM_DEFAULT_SIZE)
	, _pBegin((BYTE*)malloc(sizeof(BYTE)* NET_STREAM_DEFAULT_SIZE))
	, _pEnd(_pBegin + NET_STREAM_DEFAULT_SIZE)
	, _head(_pBegin)
	, _tail(_pBegin)
{
}

Stream::Stream(size_t size)
	: _capacity(size)
	, _pBegin((BYTE*)malloc(sizeof(BYTE)* NET_STREAM_DEFAULT_SIZE))
	, _pEnd(_pBegin + size)
	, _head(_pBegin)
	, _tail(_pBegin)
{
}

Stream::~Stream()
{
	free(_pBegin);
}

BYTE* Stream::GetReadPtr() const
{
	return _head;
}

BYTE* Stream::GetWritePtr() const
{
	return _tail;
}

size_t Stream::GetDataSize() const
{
	return _tail - _head;
}

size_t Stream::GetFreeSize() const
{
	return _pEnd - _tail;
}

bool Stream::ShiftReadPtr(size_t len)
{
	if ( GetDataSize() < len ) {
		// Buffer Overflow
		return false;
	}

	_head += len;
	return true;
}

bool Stream::ShiftWritePtr(size_t len)
{
	if ( GetFreeSize() < len ) {
		// Buffer Overflow
		return false;
	}

	_tail += len;

	return true;
}

void Stream::Rearrangement()
{
	const size_t freeSize = GetFreeSize();
	if ( _capacity >> 1 > freeSize ) {
		const size_t dataSize = GetDataSize();
		memmove(_pBegin, _head, dataSize);
		_head = _pBegin;
		_tail = _head + dataSize;
	}
}

void Stream::Flush()
{
	_head = _pBegin;
	_tail = _pBegin;
}

