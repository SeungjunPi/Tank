#pragma once


#include "stdafx.h"

#define NET_MSG_MAX_LEN (512) // 2bytes with header
#define NET_PAGE_MAX_LEN (65536)


// length
struct NetHeader
{
	UINT32 length;
};

struct NetMessage
{
	NetHeader header;
	CHAR body[NET_MSG_MAX_LEN - sizeof(NetHeader)];
};

// message length < 65536
// We do not recommend use this as message container.
struct NetPage
{
	UINT32 length;
	CHAR messages[NET_PAGE_MAX_LEN - sizeof(length)];
};


