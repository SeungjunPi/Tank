#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DNew new (_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define DNew new
#endif

#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string>

