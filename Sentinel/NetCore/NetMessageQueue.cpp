#include "NetMessageQueue.h"

NetMessageQueue::NetMessageQueue()
	: _msgBuffer(new BYTE[NET_CONCURRENT_MESSAGE_QUEUE_MAX_SIZE])
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
	// Todo: 여기 count가 문제가 생길 소지가 있음. 위 두 개가 원자적으로 동작하지 않으므로. 그런데 어차피 한 큐에 전부다 못 넣으면 이미 망한다는 마인드긴 함. 

	// 처리할 수 있는 용량이 초과됨. 버퍼를 늘리는게 가장 좋은 방법. 릴리즈에서도 크래시가 생기게 해버리는게 나을지도.
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


