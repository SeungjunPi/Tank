#include "test.h"

#include "stdlib.h"


void Foo()
{
	void* s_ptr;

	s_ptr = new char[1031];

	//s_ptr = new (_NORMAL_BLOCK, __FILE__, __LINE__) char[1032];

	s_ptr = malloc(1033);
}

void Bar(char** pPtr)
{
	*pPtr = new char[1031];
}
