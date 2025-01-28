#include "JMap.h"

const int JMap::BUCKET_SIZE = 1327;

JMap::JMap()
{
	_bucket = DNew JNode[BUCKET_SIZE];
	_bucketExtra = DNew JNode[BUCKET_SIZE];
}

JMap::~JMap()
{
	delete _bucketExtra;
	delete _bucket;
}

int JMap::GetCount() const
{
	return _count;
}

JNode* JMap::GetBegin()
{
	return _begin;
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
	UINT32 hash = key % BUCKET_SIZE;
	JNode* pNode = _bucket + hash;

	if (_count == 0) {
		
	}
	else if (_count == 1) {

	}
	
	if (pNode->_ptr == nullptr) {
		
		
		_end->_next = pNode;

		++_count;
		return;
	}

	if (pNode->_key == key) {
		return;
	}

	JNode* pNextNode = pNode->_next;

	while (true) {
		if (pNextNode == nullptr) {
			break;
		}

		if (pNextNode->_key == key) {
			return;
		}

		if (pNextNode->_key % BUCKET_SIZE != hash) {
			break;
		}
		
		pNode = pNextNode;
		pNextNode = pNextNode->_next;
	}
	
	JNode* pNewNode = GetExtraNodePtr();
	pNewNode->_key = key;
	pNewNode->_ptr = ptr;
	if (pNextNode == nullptr) {
		pNode->_next = pNewNode;
		return;
	}

	pNode->_next = pNewNode;
	pNewNode->_next = pNextNode;
}

void* JMap::Pop(UINT32 key)
{
	UINT32 hash = key % BUCKET_SIZE;
	JNode* pNode = _bucket+ hash;

	if (pNode->_ptr == nullptr) {
		return nullptr;
	}

	if (pNode->_key == key) {
		void* retVal = pNode->_ptr;

		if (_begin == pNode) {


			return retVal;
		}
		
		// yank tail
		
		return retVal;
	}


	JNode* pNextNode = pNode->_next;
	while (true) {
		if (pNextNode == nullptr) {
			return nullptr;
		}

		if (pNextNode->_key == key) {
			break;
		}

		if (pNextNode->_key % BUCKET_SIZE != hash) {
			return nullptr;
		}
	}

	void* retVal = pNextNode->_ptr;
	pNode->_next = pNextNode->_next;
	
	ReturnExtraNodePtr(pNextNode);
	return retVal;
}

JNode* JMap::GetExtraNodePtr()
{
	return nullptr;
}

void JMap::ReturnExtraNodePtr(JNode* pNode)
{
	pNode->_ptr = nullptr;
	pNode->_next = nullptr;
	pNode->_key = MAXUINT32;
}


