#pragma once

#include "pch.h"

class LiteWString 
{
public:
	static const short MAX_WSTR_LENGTH = 1000;
	static const short MAX_BUFFER_LENGTH = (MAX_WSTR_LENGTH + 1) * sizeof(WCHAR);

	LiteWString();
	LiteWString(const WCHAR* str);
	LiteWString(const WCHAR* str, short length);

	~LiteWString();

	const WCHAR* GetWString() const { return _buffer; }

	// 이 함수를 통해 획득한 버퍼에 쓴 길이를 반드시 SetLength를 통해 입력해줘야 함. 그러지 않을 시 정상 동작을 보장하지 않음. 
	WCHAR* GetWStringBuffer() { return _buffer; }
	void SetLength(short length);
	
	BOOL Append(const WCHAR* str, short length);
	BOOL Append(const WCHAR* str);
	BOOL Append(const LiteWString* pOther);
	
	BOOL Clear();

	static size_t GetLengthOf(const WCHAR* str);

private:
	short _length;
	WCHAR _buffer[LiteWString::MAX_WSTR_LENGTH + 1];
	WCHAR* _writeHead;

	

};

