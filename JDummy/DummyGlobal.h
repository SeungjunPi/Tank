#pragma once

class DummyManager;

extern DummyManager* g_pDummyManager;



class DummyGlobal
{
public:
	static void Initiate();
	static void Terminate();

private:

};

