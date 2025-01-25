#pragma once

#include <windows.h>


using UserDBIndex = UINT32;
using ObjectKey = UINT16; // EGameObjectType의 크기를 변경한다면 여기를 반드시 변경해줘야 함.

const UserDBIndex INVALID_USER_ID = 0xFFFFFFFF;

