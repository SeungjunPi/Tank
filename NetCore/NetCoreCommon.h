#pragma once



enum class ESessionEvent
{
	NONE,
	CREATE_PASSIVE_CLIENT,
	DELETE_PASSIVE_CLIENT,
	CREATE_ACTIVE_SERVER,
};


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


enum ESessionRemoveReason
{
	COMPLETION_RECV_ERROR,
	COMPLETION_SEND_ERROR,
	COMPLETION_RECV_DISCONNECT,
	FORCE_REMOVE
};

