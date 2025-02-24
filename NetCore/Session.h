#pragma once

#include "NetCorePch.h"
#include "NetMessage.h"
#include "Stream.h"
#include "NetCoreCommon.h"

class SessionManager;

// SessionManager 클래스에서 관리돼야 하며, 외부에서 직접 접근할 시 쓰레드 안전을 보장하지 않음.
class Session
{
	friend SessionManager;
public:
	Session(SOCKET socket, SessionID id);
	~Session();

	void Initiate();
	void Terminate();

	UINT32 GetID() const;
	SOCKET GetSocket() const;

	// Receive가 정상적으로 pending 됐으면 true, 아니면 false 반환.
	// 쓰레드 안전하지 않은 함수.
	bool RegisterReceive();
	
	// Receive가 정상적으로 pending 됐으면 true, 아니면 false 반환.
	// false를 반환받을 시 즉시 GetLastError()를 통해 원인을 파악할 것.
	bool RegisterSend();

	void OnReceive(DWORD length);

	// 여기서 얻은 NetMessage 포인터에 대해 메모리 할당 해제를 하면 안됨.
	NetMessage* GetReceiveNetMessageOrNull();

	void WriteSendMessage(BYTE* msg, UINT32 len);
	
	bool TrySwapSendPages();
	
	void ResetSendFrontPage();

	void CloseSocket() const;

	static void HandleWSAError(DWORD errorCode, SessionID sessionId);

private:
	SessionID _id = 0;
	SOCKET _socket = 0;

	Stream* _receiveStream = nullptr;
	WSABUF _receiveWsaBuf = { 0, };
	IoOperationData _receiveIoData = { 0, };


	NetPage* _sendFrontNetPage = nullptr;
	NetPage* _sendBackNetPage = nullptr;

	WSABUF _sendWsaBuf = { 0, };
	IoOperationData _sendIoData = { 0, };
};

