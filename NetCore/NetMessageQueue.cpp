#include "NetMessageQueue.h"

NetMessageQueue::NetMessageQueue()
	: _msgBuffer(DNew BYTE[NET_CONCURRENT_MESSAGE_QUEUE_MAX_SIZE])
	, _writeIndex(0)
	, _readIndex(0)
	, _count(0)
{

}

NetMessageQueue::~NetMessageQueue()
{
	delete[] _msgBuffer;
}

void NetMessageQueue::WriteNetMessage(UINT32 sessionID, NetMessage* pMessage)
{
	assert(pMessage != NULL);
	size_t sizeNetMessage = pMessage->header.length + sizeof(NetMessage::header);
	LONG count = InterlockedExchangeAdd(&_count, 1);
	LONG writeIndex = InterlockedExchangeAdd(&_writeIndex, sizeof(sessionID) + sizeNetMessage);

	// 처리할 수 있는 용량이 초과됨. 버퍼를 늘리는게 가장 좋은 방법.
	// 게임에서 사용하는 패킷의 양은 대개 정해져있으므로, 아래 경우는 오류의 상황이라 간주해도 무방. 
	assert(writeIndex < NET_CONCURRENT_MESSAGE_QUEUE_MAX_SIZE);

	BYTE* pWrite = _msgBuffer + writeIndex;
	*(UINT32*)pWrite = sessionID;
	pWrite += sizeof(sessionID);

	memcpy(pWrite, pMessage, sizeNetMessage);
}

NetMessage* NetMessageQueue::GetNetMessageOrNull(UINT32* out_sessionID)
{
	assert(out_sessionID != NULL);
	if ( _readIndex == _writeIndex ) {
		return NULL;
	}

	BYTE* pRead = _msgBuffer + _readIndex;

	*out_sessionID = *(UINT32*)pRead;
	pRead += sizeof(UINT32);
	_readIndex += sizeof(UINT32);

	NetMessage* pMsg = (NetMessage*)(_msgBuffer + _readIndex);
	_readIndex += pMsg->header.length + sizeof(NetMessage::header);
	--_count;

	return pMsg;
}

void NetMessageQueue::Flush()
{
	_readIndex = 0;
	_writeIndex = 0;
	_count = 0;
}


