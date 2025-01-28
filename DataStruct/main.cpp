
#include <vector>
#include <cstdio>
#include "JMap.h"


void PrintMapLinear(JMap* pMap);
void PrintMapIterator(JNode* begin, JNode* end);


void PrintMapLinear(JMap* pMap)
{
	JNode* begin = pMap->GetBegin();
	JNode* end = pMap->GetEnd();

	PrintMapIterator(begin, end);
}

void PrintMapIterator(JNode* begin, JNode* end)
{
	JNode* node = begin;
	if (begin == end) {
		printf("No members in the map\n");
		return;
	}
	
	while (node != end) {
		printf("Key:%lu ", node->GetKey());
		node = node->Next();
	}
	printf("\n");
}


int main()
{
	// JMap 테스트
	JMap map;

	
	UINT32 keys[10] = { 0, };
	JNode* begin = nullptr;
	JNode* end = map.GetEnd();
	{
		// 첫 번째로 삽입/삭제.
		keys[0] = 0;
		keys[1] = 1327;
		keys[2] = 1326;
		for (int i = 0; i < 3; ++i) {
			map.Emplace(keys[i], &keys[i]);

			begin = map.GetBegin();
			PrintMapIterator(begin, end);

			bool res = &keys[i] == map.Pop(keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
	}


	{
		keys[0] = 0;
		keys[1] = 1327;

		for (int i = 0; i < 2; ++i) {
			map.Emplace(keys[i], &keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
		
		
		for (int i = 0; i < 2; ++i) {
			bool res = &keys[i] == map.Pop(keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}

		for (int i = 0; i < 2; ++i) {
			map.Emplace(keys[i], &keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
		for (int i = 1; i >= 0; --i) {
			bool res = &keys[i] == map.Pop(keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
	}

	{
		keys[0] = 0;
		keys[1] = 1326;

		for (int i = 0; i < 2; ++i) {
			map.Emplace(keys[i], &keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
		for (int i = 0; i < 2; ++i) {
			bool res = &keys[i] == map.Pop(keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}

		for (int i = 0; i < 2; ++i) {
			map.Emplace(keys[i], &keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
		
		for (int i = 0; i < 2; ++i) {
			bool res = &keys[i] == map.Pop(keys[i]);
			begin = map.GetBegin();
			PrintMapIterator(begin, end);
		}
	}

	{
		keys[0] = 0;
		keys[1] = 1327;
		keys[2] = 1;

		for (int i = 0; i < 3; ++i) {
			map.Emplace(keys[i], &keys[i]);
			PrintMapLinear(&map);
		}

		void* p1 = map.Pop(keys[1]);
		PrintMapLinear(&map);

		map.Emplace(keys[1], &keys[1]);
		PrintMapLinear(&map);

		p1 = map.Pop(keys[0]);
		PrintMapLinear(&map);

		map.Pop(keys[1]);
		map.Pop(keys[2]);
	}

	{
		keys[0] = 0;
		keys[1] = 1326;
		keys[2] = 1;

		for (int i = 0; i < 3; ++i) {
			map.Emplace(keys[i], &keys[i]);
		}
		PrintMapLinear(&map);

		void* p1 = map.Pop(keys[1]);
		PrintMapLinear(&map);

		map.Emplace(keys[1], &keys[1]);
		PrintMapLinear(&map);

		p1 = map.Pop(keys[0]); // begin 삭제
		PrintMapLinear(&map);
	}

	{
		JMap newMap;
		keys[0] = 0;
		keys[1] = 1327;
		keys[2] = 1326;
		keys[3] = 1327 * 2;
		keys[4] = 1327 * 3;

		for (int i = 0; i < 5; ++i) {
			newMap.Emplace(keys[i], &keys[i]);
			PrintMapLinear(&newMap);
		}
		
	}


	return 0;
}