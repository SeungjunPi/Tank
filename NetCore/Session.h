#pragma once

#include "NetCorePch.h"
#include "NetMessage.h"
#include "Stream.h"
#include "NetCoreCommon.h"

enum ESessionRefParam
{
	SESSION_REF_DECREASE_RECV,
	SESSION_REF_DECREASE_SEND
};

enum ESessionRefResult
{
	SESSION_REF_DEACTIVATE,
	SESSION_REF_STOP_RECVING,
	SESSION_REF_STOP_SENDING,
};

// SessionManager 클래스에서 관리돼야 하며, 외부에서 직접 접근할 시 쓰레드 안전을 보장하지 않음.
class Session
{
public:
	Session(SOCKET socket, SessionID id);
	~Session();

	void Initiate();
	void Terminate();

	UINT32 GetID() const;
	SOCKET GetSocket() const;

	void Disconnect();

	void RegisterReceive();
	void OnReceive(DWORD length);

	// 여기서 얻은 NetMessage 포인터에 대해 메모리 할당 해제를 하면 안됨.
	NetMessage* GetReceiveNetMessageOrNull();

	// 반드시 외부에서 lock을 통해 쓰레드안전하게 만든 후 호출해야 함
	void Send(BYTE* msg, UINT32 len);
	
	void OnSendComplete();

	

	ESessionRefResult ReduceReference(ESessionRefParam param);

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

	
	BOOL _isSending = false;
	BOOL _isRecving = true;

	void RegisterSend();
};

