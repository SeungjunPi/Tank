#include "LiteWString.h"

LiteWString::LiteWString()
	: _length(0)
	, _writeHead(_buffer)
{
	memset(_buffer, 0, LiteWString::MAX_BUFFER_LENGTH);
}

LiteWString::LiteWString(const WCHAR* str)
{
	memset(_buffer, 0, LiteWString::MAX_BUFFER_LENGTH);
	size_t length = LiteWString::GetLengthOf(str);
	_length = (short)length;

	memcpy(_buffer, str, sizeof(WCHAR) * _length);
}

LiteWString::LiteWString(const WCHAR* wstr, short length)
	: _length(length)
	, _writeHead(_buffer)
{
	assert(length < LiteWString::MAX_WSTR_LENGTH);
	memset(_buffer, 0, LiteWString::MAX_BUFFER_LENGTH);
	memcpy(_buffer, wstr, sizeof(WCHAR) * length);
	_writeHead += length;
}

LiteWString::~LiteWString()
{
}

BOOL LiteWString::Append(const WCHAR* str, short length)
{
	if (_length + length > MAX_WSTR_LENGTH) {
		return false;
	}

	memcpy(_writeHead, str, length * sizeof(WCHAR));
	_length += length;
	_writeHead += length;
	*_writeHead = L'\0';

	return true;
}

BOOL LiteWString::Append(const WCHAR* str)
{
	size_t length = LiteWString::GetLengthOf(str);
	BOOL result = Append(str, (short)length);
	return result;
}

BOOL LiteWString::Append(const LiteWString* pOther)
{
	if (_length + pOther->_length > MAX_WSTR_LENGTH) {
		return false;
	}

	Append(pOther->GetWString(), pOther->_length);
	return true;
}

BOOL LiteWString::Clear()
{
	memset(_buffer, 0, MAX_BUFFER_LENGTH);
	_writeHead = _buffer;
	_length = 0;

	return 0;
}

size_t LiteWString::GetLengthOf(const WCHAR* str)
{
	const WCHAR* ptr = str;
	size_t length = 0;
	for (; length <= MAX_WSTR_LENGTH; ++length) {
		if (*ptr == L'\0') {
			break;
		}
		++ptr;
	}

	if (length > MAX_WSTR_LENGTH) {
		// Critical Error
		__debugbreak();
		return 0;
	}

	return length;
}

void LiteWString::SetLength(short length)
{
	_length = length;
	_writeHead = _buffer + _length;
	*_writeHead = L'\0';
}

