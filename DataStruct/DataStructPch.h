
#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <SDKDDKVer.h>

#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <process.h>
#include <cassert>
#include <sqlext.h>
#include <sqltypes.h>
#include <sql.h>
