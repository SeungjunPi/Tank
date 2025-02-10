#pragma once

#include "NetCorePch.h"
#include "NetCoreCommon.h"

#define NETCORE_API __declspec(dllexport)
#define NET_SESSION_EVENT_QUEUE_MAX_SIZE (65536)


// 쓰는 것은 동시에 가능하나, 읽는 동작은 별도로 싱글스레드에서 처리해야 한다.
class NETCORE_API NetSessionEventQueue
{
	// Todo: Buffer를 Pool에서 가져오기.
public:
	NetSessionEventQueue();
	~NetSessionEventQueue();

	// Net 내부에서만 호출할 함수
	void WriteNetSessionEvent(UINT32 sessionID, ESessionEvent sessionEvent);

	// Net 외부에서만 호출할 함수
	ESessionEvent GetNetSessionEvent(UINT32* out_sessionID);

	void Flush();

private:
	BYTE* _msgBuffer;

	LONG _count;
	LONG _writeIndex;
	LONG _readIndex;
};
