#include "NetSessionEventQueue.h"

NetSessionEventQueue::NetSessionEventQueue()
	: _msgBuffer(new BYTE[NET_SESSION_EVENT_QUEUE_MAX_SIZE])
	, _writeIndex(0)
	, _readIndex(0)
	, _count(0)
{

}

NetSessionEventQueue::~NetSessionEventQueue()
{
	delete[] _msgBuffer;
}

void NetSessionEventQueue::WriteNetSessionEvent(UINT32 sessionID, ESessionEvent sessionEvent)
{
	LONG count = InterlockedExchangeAdd(&_count, 1);
	LONG writeIndex = InterlockedExchangeAdd(&_writeIndex, sizeof(sessionID) + sizeof(ESessionEvent));

	// 처리할 수 있는 용량이 초과됨. 버퍼를 늘리는게 가장 좋은 방법. 릴리즈에서도 크래시가 생기게 해버리는게 나을지도.
	assert(writeIndex < NET_SESSION_EVENT_QUEUE_MAX_SIZE);

	BYTE* pWrite = _msgBuffer + writeIndex;
	*(UINT32*)pWrite = sessionID;
	pWrite += sizeof(sessionID);
	*(ESessionEvent*)pWrite = sessionEvent;
}

ESessionEvent NetSessionEventQueue::GetNetSessionEvent(UINT32* out_sessionID)
{
	assert(out_sessionID != NULL);
	if (_readIndex == _writeIndex) {
		return ESessionEvent::NONE;
	}

	BYTE* pRead = _msgBuffer + _readIndex;

	*out_sessionID = *(UINT32*)pRead;
	pRead += sizeof(UINT32);
	_readIndex += sizeof(UINT32);

	ESessionEvent sessionEvent = *(ESessionEvent*)(_msgBuffer + _readIndex);
	_readIndex += sizeof(ESessionEvent);
	--_count;

	return sessionEvent;
}

void NetSessionEventQueue::Flush()
{
	_readIndex = 0;
	_writeIndex = 0;
	_count = 0;
}


