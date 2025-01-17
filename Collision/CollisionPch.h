#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DNew new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DNew new
#endif

#include <SDKDDKVer.h>

#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <tchar.h>
#include <process.h>
#include <cassert>

