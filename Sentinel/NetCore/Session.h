#pragma once

#include "stdafx.h"
#include "NetMessage.h"
#include "Stream.h"

struct IoOperationData
{
	WSAOVERLAPPED wol;
	enum {
		SEND,
		RECEIVE,
		ACCEPT,
	} operation;
	SOCKET socket;
};

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


class Session
{
public:
	Session(SOCKET socket, SHORT id);
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

	void SendLock(BYTE* msg, UINT32 len);

	// 반드시 외부에서 lock을 통해 쓰레드안전하게 만든 후 호출해야 함
	void Send(BYTE* msg, UINT32 len);
	
	void OnSendComplete();
	void OnSendCompleteImediately(); // deprecated

	ESessionRefResult ReduceReference(ESessionRefParam param);


	void Lock();
	void Unlock();
	

	

private:
	UINT32 _id = 0;
	SOCKET _socket = 0;

	Stream* _receiveStream = nullptr;
	WSABUF _receiveWsaBuf = { 0, };
	IoOperationData _receiveIoData = { 0, };


	NetPage* _sendFrontNetPage = nullptr;
	NetPage* _sendBackNetPage = nullptr;

	WSABUF _sendWsaBuf = { 0, };
	IoOperationData _sendIoData = { 0, };

	SRWLOCK _srwLock = SRWLOCK_INIT;
	
	BOOL _isSending = false;
	BOOL _isRecving = true;

	void RegisterSend();
};

