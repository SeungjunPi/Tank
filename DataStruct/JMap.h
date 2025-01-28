#pragma once

#include "DataStructPch.h"

#include "JNode.h"



class JMap
{
public:
	JMap();
	~JMap();

	int GetCount() const;

	// key의 크기와 무관함
	JNode* GetBegin();
	void* Get(UINT32 key);
	void Emplace(UINT32 key, void* ptr);
	void* Pop(UINT32 key);

private:
	static const int BUCKET_SIZE;

	int _count = 0;

	JNode* _begin = nullptr;
	JNode* _end = nullptr;

	JNode* _bucket = nullptr;
	JNode* _bucketExtra = nullptr;


	JNode* GetExtraNodePtr();
	void ReturnExtraNodePtr(JNode* pNode);

	
};
