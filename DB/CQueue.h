#pragma once

#include "pch.h"
#include <queue>

using namespace std;

template <class T> 
class CQueue
{
public:
	CQueue();
	~CQueue();

	inline bool empty() 
	{ 
		AcquireSRWLockExclusive(&_srwLock);
		bool res = _queue.empty();
		ReleaseSRWLockExclusive(&_srwLock);
		return res; 
	}

	inline void Push(T t) 
	{ 
		AcquireSRWLockExclusive(&_srwLock);
		_queue.push(t);
		ReleaseSRWLockExclusive(&_srwLock);
	}

	inline bool TryGetAndPop(T* out) 
	{
		bool success = false;
		AcquireSRWLockExclusive(&_srwLock);
		if (!_queue.empty()) {
			success = true;
			*out = _queue.front();
		}
		ReleaseSRWLockExclusive(&_srwLock);
		return success;
	}

private:
	queue<T> _queue;
	SRWLOCK _srwLock;
};

template<class T> CQueue<T>::CQueue()
	: _queue()
	, _srwLock(SRWLOCK_INIT)
{
	
}

template<class T> CQueue<T>::~CQueue()
{
	
}
