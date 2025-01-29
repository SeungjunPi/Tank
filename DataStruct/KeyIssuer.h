#pragma once


#include "DataStructPch.h"
#include "LinearQueue.h"

// UINT32 타입의 Key를 중복없이 발급하는 클래스.
// 0부터 MAXUINT32-1 까지 순차적으로 발급함.
// 누적해서 21억개를 초과해서 사용하게 되는 상황에 대응해야한다면, Key를 64비트로 늘리거나 사용하고 있는 Key를 체크하는 로직이 필요해짐. 
class KeyIssuer32
{
public:
	UINT32 IssueKey() { return unusedKey++; }
private:
	UINT32 unusedKey = 0;
};