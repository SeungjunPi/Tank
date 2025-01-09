#pragma once

class PointerQueue
{
public:
	PointerQueue(int capacity);
	~PointerQueue();


	bool Enqueue(void* entry);
	void* Dequeue();

	int GetCount() const;

private:
	const int _capacity;
	void** _entries;
	int _count;

	int _head;
	int _tail;

	void Rearrangement();
};