#include "Session.h"

Session::Session(SOCKET socket, SessionID id)
	: _socket(socket)
	, _id(id)
{
}

Session::~Session()
{
	_socket = 0;
	_id = INVALID_SESSION_ID;
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
	if (_isRecving || _isSending) {
		__debugbreak();
	}
	assert(_socket == INVALID_SOCKET);
	
	delete _sendFrontNetPage;
	delete _sendBackNetPage;
	delete _receiveStream;
}

DWORD Session::RegisterReceive()
{
	DWORD dwFlag = 0;
	DWORD numBytes = 0;

	_receiveStream->Rearrangement();

	_receiveWsaBuf.buf = (CHAR*)_receiveStream->GetWritePtr();
	_receiveWsaBuf.len = (ULONG)_receiveStream->GetFreeSize();
	DWORD result = WSARecv(_socket, &_receiveWsaBuf, 1, &numBytes, &dwFlag, &_receiveIoData.wol, NULL);
	if (result == 0) {
		// Send가 즉시 완료되었으나, Overlapped socket이므로 GQCS에서 처리됨
		// 외부에선 IO Complete에서 처리할지 말지만 알면 되므로 PENDING을 반환.
		return WSA_IO_PENDING;
	}

	DWORD errorCode = WSAGetLastError();
	if (errorCode == WSA_IO_PENDING) {
		// 정상적으로 예약됨, GQCS에서 처리될 예정
		return ERROR_IO_PENDING;
	}

	HandleWSAError(errorCode, _id);
	return result;
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

DWORD Session::RegisterSend()
{
	_sendWsaBuf.buf = (CHAR*)_sendFrontNetPage->messages;
	_sendWsaBuf.len = _sendFrontNetPage->length;

	int result = WSASend(_socket, &_sendWsaBuf, 1, NULL, 0, &_sendIoData.wol, NULL);
	if (result == 0) {
		// Send가 즉시 완료되었으나, Overlapped socket이므로 GQCS에서 처리됨
		return ERROR_IO_PENDING;
	}
	else if (result == SOCKET_ERROR) {
		DWORD error = GetLastError();
		if (error == ERROR_IO_PENDING) {
			// 정상적으로 예약됨, GQCS에서 처리될 예정
			return ERROR_IO_PENDING;
		}

		HandleWSAError(error, _id);
		return error;
	}

	__debugbreak();
}

bool Session::Send(BYTE* msg, UINT32 len)
{
	bool isAlreadySending = false;
	
	if (!_isRecving) {
		assert(_isSending);
		// Receiving이 종료됐으나, Sending이 종료되지 않은 상황. 
		// Sending에서 처리될 것이므로, 종료.
		// 둘 모두 false인 경우, manager에서 이미 걸러졌어야 함. 
		return false;
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
		return true;
	}

	DWORD wsaResult = RegisterSend();
	if (wsaResult == ERROR_IO_PENDING) {
		return true;
	}

	return false;
}

void Session::OnSendComplete()
{
	_sendFrontNetPage->length = 0;
}

bool Session::TryResend()
{
	if (_sendBackNetPage->length == 0) {
		_isSending = false;
		return true;
	}
	
	NetPage* tmp = _sendBackNetPage;
	_sendBackNetPage = _sendFrontNetPage;
	_sendFrontNetPage = tmp;
	DWORD sendResult = RegisterSend();
	if (sendResult == ERROR_IO_PENDING) {
		_isSending = true;
		return true;
	}
	return false;
}

ESessionRefResult Session::ReduceReference(ESessionRefParam param)
{
	ESessionRefResult result = SESSION_REF_DEACTIVATE;
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
	
	return result;
}

void Session::HandleWSAError(DWORD errorCode, SessionID sessionId)
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
	_socket = INVALID_SOCKET;
}



