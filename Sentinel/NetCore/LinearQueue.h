#pragma once


class LinearQueue
{
public:
	void Initiate(int elementSize, int capacity);
	void Terminate();

	bool Push(void* pSrc);
	void TryPopTo(void* out);

	bool Remove(void* pSrc);

	int GetCount() const;

	// Use this for iterator
	char* GetHead() const;

private:
	char* _pElement = nullptr;

	char* _head = nullptr;
	char* _tail = nullptr;

	char* _pEnd = nullptr;

	int _elementSize = 0;
	int _capacity = 0;
	int _numElements = 0;
};

