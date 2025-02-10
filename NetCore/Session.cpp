#include "Session.h"

Session::Session(SOCKET socket, SHORT id)
	: _socket(socket)
	, _id(id)
{
}

Session::~Session()
{
	_socket = 0;
	_id = 0xFFFFFFFF;
}

void Session::Initiate()
{
	_receiveStream = DNew Stream;

	_receiveWsaBuf.buf = NULL;
	_receiveWsaBuf.len = 0;
	_receiveIoData.operation = IoOperationData::RECEIVE;


	_sendFrontNetPage = DNew NetPage;
	_sendFrontNetPage->length = 0;
	_sendBackNetPage = DNew NetPage;
	_sendBackNetPage->length = 0;
	_sendWsaBuf.buf = NULL;
	_sendWsaBuf.len = 0;
	_sendIoData.operation = IoOperationData::SEND;
	_isRecving = true;
}

void Session::Terminate()
{
	AcquireSRWLockExclusive(&_srwLock);
	if (_isRecving || _isSending) {
		__debugbreak();
	}
	if (_socket == INVALID_SOCKET) {
		return;
	}
	closesocket(_socket);
	_socket = INVALID_SOCKET;
	
	delete _sendFrontNetPage;
	delete _sendBackNetPage;
	delete _receiveStream;

	ReleaseSRWLockExclusive(&_srwLock);
}

void Session::RegisterReceive()
{
	DWORD dwFlag = 0;
	DWORD numBytes = 0;

	_receiveStream->Rearrangement();

	_receiveWsaBuf.buf = (CHAR*)_receiveStream->GetWritePtr();
	_receiveWsaBuf.len = (ULONG)_receiveStream->GetFreeSize();
	DWORD result = WSARecv(_socket, &_receiveWsaBuf, 1, &numBytes, &dwFlag, &_receiveIoData.wol, NULL);
	if (result == 0) {
		// Send가 즉시 완료되었으나, Overlapped socket이므로 GQCS에서 처리됨
		return;
	}

	DWORD errorCode = WSAGetLastError();
	if (errorCode == WSA_IO_PENDING) {
		// 정상적으로 예약됨, GQCS에서 처리될 예정
		return;
	}

	HandleWSAError(errorCode, _id);
}

void Session::OnReceive(DWORD length)
{
	_receiveStream->ShiftWritePtr(length);
}

NetMessage* Session::GetReceiveNetMessageOrNull()
{
	size_t streamLength = _receiveStream->GetDataSize();
	if ( streamLength < sizeof(NetMessage::header) ) {
		return NULL;
	}

	NetMessage* receiveMessage = (NetMessage*)_receiveStream->GetReadPtr();
	if ( streamLength >= receiveMessage->header.length ) {
		_receiveStream->ShiftReadPtr(receiveMessage->header.length + sizeof(NetMessage::header));
		return receiveMessage;
	}
	
	return NULL;
}

void Session::SendLock(BYTE* msg, UINT32 len)
{
	bool isAlreadySending = false;
	AcquireSRWLockExclusive(&_srwLock);

	if (!_isRecving) {
		Terminate();
		ReleaseSRWLockExclusive(&_srwLock);
		return;
	}

	UINT32 writePos = _sendBackNetPage->length;
	NetMessage* pNetMessage = (NetMessage*)(_sendBackNetPage->messages + writePos);
	pNetMessage->header.length = len;
	memcpy(pNetMessage->body, msg, len);
	_sendBackNetPage->length += len + sizeof(pNetMessage->header);
	
	isAlreadySending = _isSending;
	_isSending = true;
	if (!isAlreadySending) {
		NetPage* tmp = _sendBackNetPage;
		_sendBackNetPage = _sendFrontNetPage;
		_sendFrontNetPage = tmp;
	}

	ReleaseSRWLockExclusive(&_srwLock);
	
	if (isAlreadySending) {
		return;
	}

	RegisterSend();
}

void Session::RegisterSend()
{
	_sendWsaBuf.buf = (CHAR*)_sendFrontNetPage->messages;
	_sendWsaBuf.len = _sendFrontNetPage->length;

	int result = WSASend(_socket, &_sendWsaBuf, 1, NULL, 0, &_sendIoData.wol, NULL);
	if (result == 0) {
		// Send가 즉시 완료되었으나, Overlapped socket이므로 GQCS에서 처리됨
		return;
	}
	else if (result == SOCKET_ERROR) {
		DWORD error = GetLastError();
		if (error == ERROR_IO_PENDING) {
			// 정상적으로 예약됨, GQCS에서 처리될 예정
			return;
		}

		HandleWSAError(error, _id);
		Disconnect();
		
		// Todo: Handle error	
		return;
	}

	__debugbreak();
}

void Session::Send(BYTE* msg, UINT32 len)
{
	bool isAlreadySending = false;
	
	if (!_isRecving) {
		Terminate();
		return;
	}

	UINT32 writePos = _sendBackNetPage->length;
	NetMessage* pNetMessage = (NetMessage*)(_sendBackNetPage->messages + writePos);
	pNetMessage->header.length = len;
	memcpy(pNetMessage->body, msg, len);
	_sendBackNetPage->length += len + sizeof(pNetMessage->header);

	isAlreadySending = _isSending;
	_isSending = true;
	if (!isAlreadySending) {
		NetPage* tmp = _sendBackNetPage;
		_sendBackNetPage = _sendFrontNetPage;
		_sendFrontNetPage = tmp;
	}

	if (isAlreadySending) {
		return;
	}

	RegisterSend();
}

void Session::OnSendComplete()
{
	bool sendAgain = false;

	_sendFrontNetPage->length = 0;
	AcquireSRWLockExclusive(&_srwLock);
	if (!_isRecving) {
		// IO에서 error가 발생하진 않았으나, session이 비활성화되어 receive가 비활성화 된 경우. 
		// Todo: 이 부분을 확인하지 않고 GQCS의 콜백에서 처리해도 괜찮을지 검토 
		_isSending = false;
		Terminate();
	} else if (_sendBackNetPage->length != 0) {
		sendAgain = true;
		NetPage* tmp = _sendBackNetPage;
		_sendBackNetPage = _sendFrontNetPage;
		_sendFrontNetPage = tmp;
	}
	else {
		_isSending = false;
	}
	ReleaseSRWLockExclusive(&_srwLock);

	if (sendAgain) {
		RegisterSend();
		return;
	}
}

ESessionRefResult Session::ReduceReference(ESessionRefParam param)
{
	ESessionRefResult result = SESSION_REF_DEACTIVATE;
	AcquireSRWLockExclusive(&_srwLock);
	switch (param) {
	case SESSION_REF_DECREASE_RECV:
		_isRecving = false;
		result = SESSION_REF_STOP_RECVING;
		break;
	case SESSION_REF_DECREASE_SEND:
		_isSending = false;
		result = SESSION_REF_STOP_SENDING;
		break;
	}

	if (!_isRecving && !_isSending) {
		result = SESSION_REF_DEACTIVATE;
	}
	ReleaseSRWLockExclusive(&_srwLock);
	return result;
}

void Session::Lock()
{
	AcquireSRWLockExclusive(&_srwLock);
}

void Session::Unlock()
{
	ReleaseSRWLockExclusive(&_srwLock);
}

void Session::HandleWSAError(DWORD errorCode, UINT32 sessionId)
{
	// Todo: handle errors
	printf("Send error occured, errorCode=%d, sessionId=%u\n", errorCode, sessionId);
}

UINT32 Session::GetID() const
{
	return _id;
}

SOCKET Session::GetSocket() const
{
	return _socket;
}

void Session::Disconnect()
{
	closesocket(_socket);
}



