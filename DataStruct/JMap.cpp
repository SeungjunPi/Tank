#include "JMap.h"

const int JMap::BUCKET_SIZE = 1327;

JMap::JMap()
{
	_bucket = DNew JNode[BUCKET_SIZE + 1];
	_bucketExtra = DNew JNode[BUCKET_SIZE];

	_end = _bucket + BUCKET_SIZE;
}

JMap::~JMap()
{
	delete[] _bucketExtra;
	delete[] _bucket;
}

int JMap::GetCount() const
{
	return _count;
}

JNode* JMap::GetBegin()
{
	if (_begin == nullptr) {
		return _end;
	}
	return _begin;
}

JNode* JMap::GetEnd()
{
	return _end;
}

void* JMap::Get(UINT32 key)
{
	UINT32 hash = key % BUCKET_SIZE;
	JNode* pNode = _bucket + hash;

	while (pNode->_key != key) {
		pNode = pNode->_next;

		if (pNode == nullptr) {
			return nullptr;
		}
	}

	return pNode->_ptr;
}

void JMap::Emplace(UINT32 key, void* ptr)
{
	if (_count == BUCKET_SIZE * 2) {
		// BUCKET SIZE를 늘릴 것을 고려해볼 것. 
		__debugbreak();
	}
	UINT32 hash = key % BUCKET_SIZE;
	JNode* pNode = _bucket + hash;

	if (_begin == nullptr) {
		// first node
		_begin = pNode;
		_begin->_key = key;
		_begin->_ptr = ptr;
		_begin->_next = _end;
		_end->_prev = _begin;
		++_count;
		return;
	}

	if (pNode->_ptr == nullptr) {
		// inside of bucket
		JNode* pTailNode = _end->_prev;
		pTailNode->_next = pNode;
		pNode->_prev = pTailNode;
		pNode->_next = _end;
		_end->_prev = pNode;

		pNode->_key = key;
		pNode->_ptr = ptr;

		++_count;
		return;
	}

	// Duplicated Hash
	while (pNode != _end) {
		if (pNode->_key == key) {
			return;
		}

		if (pNode->_key % BUCKET_SIZE != hash) {
			break;
		}
		pNode = pNode->_next;
	}

	// pNode는 _end이거나 hash 라인 직후의 노드. 
	// [prevNode] -> [pNode] => [prevNode] -> [newNode] -> [pNode]
	
	JNode* pNewNode = GetExtraNodePtr();
	JNode* pPrevNode = pNode->_prev;

	pPrevNode->_next = pNewNode;
	pNewNode->_prev = pPrevNode;
	pNewNode->_next = pNode;
	pNode->_prev = pNewNode;

	pNewNode->_key = key;
	pNewNode->_ptr = ptr;
	++_count;
	return;
}

void* JMap::Pop(UINT32 key)
{
	UINT32 hash = key % BUCKET_SIZE;
	JNode* pNode = _bucket+ hash;

	if (pNode->_ptr == nullptr) {
		return nullptr;
	}

	if (_count == 1) {
		if (_begin->_key != key) {
			return nullptr;
		}

		void* ptr = pNode->_ptr;
		_begin = nullptr;
		_end->_prev = nullptr;
		pNode->Reset();
		--_count;
		return ptr;
	}

	if (pNode->_key == key) {
		// bucket
		if (pNode->_next == _end || pNode->_next->_key % BUCKET_SIZE != hash) {
			// no extra
			if (pNode == _begin) {
				_begin = pNode->_next;
				_begin->_prev = nullptr;
			}
			else {
				RemoveMiddleNode(pNode);
			}
			
			void* ret = pNode->_ptr;
			pNode->Reset();
			--_count;
			return ret;
		}

		// exist extra
		JNode* pTail = pNode->_next;
		while (pTail->_next != _end) {
			if (pTail->_next->_key % BUCKET_SIZE != hash) {
				break;
			}
			pTail = pTail->_next;
		}

		pNode->SwapData(pTail);
		void* ret = pTail->_ptr;
		RemoveMiddleNode(pTail);
		pTail->Reset();
		ReturnExtraNodePtr(pTail);
		--_count;
		return ret;
	}

	// extra
	pNode = pNode->_next;
	while (pNode->_key != key) {
		if (pNode == _end) {
			return nullptr;
		}
		if (pNode->_key % BUCKET_SIZE != hash) {
			return nullptr;
		}
		pNode = pNode->_next;
	}

	RemoveMiddleNode(pNode);
	void* ret = pNode->_ptr;
	pNode->Reset();
	ReturnExtraNodePtr(pNode);
	--_count;
	return ret;
}

JNode* JMap::GetExtraNodePtr()
{
	// 선형탐색, 필요시 index를 queue에 넣는 방식으로 변경. 
	JNode* pNode = _bucketExtra;
	for (int i = 0; i < BUCKET_SIZE; ++i) {
		if (pNode->_ptr == nullptr) {
			return pNode;
		}
		++pNode;
	}
	__debugbreak();
	return nullptr;
}

void JMap::ReturnExtraNodePtr(JNode* pNode)
{
	pNode->_ptr = nullptr;
	pNode->_next = nullptr;
	pNode->_key = MAXUINT32;
}

void JMap::RemoveMiddleNode(JNode* pNode)
{
	JNode* pPrev = pNode->_prev;
	JNode* pNext = pNode->_next;

	pPrev->_next = pNext;
	pNext->_prev = pPrev;
}


