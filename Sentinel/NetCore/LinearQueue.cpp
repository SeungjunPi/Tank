#include "stdafx.h"

#include "LinearQueue.h"

void LinearQueue::Initiate(int elementSize, int capacity)
{
	_elementSize = elementSize;
	_capacity = capacity;
	_numElements = 0;

	_pElement = (char*)malloc(elementSize * capacity);
	if (_pElement == nullptr) {
		__debugbreak();
	}
	_head = _pElement;
	_tail = _head;
	_pEnd = _pElement + elementSize * capacity;
}

void LinearQueue::Terminate()
{
	free(_pElement);
	_pElement = nullptr;
	_head = nullptr;
	_tail = nullptr;
}

bool LinearQueue::Push(void* pSrc)
{
	if (_numElements == _capacity) {
		return false;
	}

	if (_tail == _pEnd) {
		memmove(_pElement, _head, _elementSize * _numElements);
		_head = _pElement;
		_tail = _head + _elementSize * _numElements;
	}

	memcpy(_tail, pSrc, _elementSize);
	_tail += _elementSize;
	++_numElements;
	return true;
}

void LinearQueue::TryPopTo(void* out)
{
	if (_head == _tail) {
		memset(out, 0xFF, sizeof(_elementSize));
		return;
	}

	memcpy(out, _head, _elementSize);
	_head += _elementSize;
	--_numElements;
}

bool LinearQueue::Remove(void* pSrc)
{
	for (int i = 0; i < _numElements; ++i) {
		int res = memcmp(pSrc, _head + i * _elementSize, _elementSize);
		if (res == 0) {
			_tail -= _elementSize;
			memcpy(_head + i * _elementSize, _tail, _elementSize);
			memset(_tail, 0, _elementSize);
			_numElements--;
			return true;
		}
	}

	return false;
}

int LinearQueue::GetCount() const
{
	return _numElements;
}

char* LinearQueue::GetHead() const
{
	return _head;
}




