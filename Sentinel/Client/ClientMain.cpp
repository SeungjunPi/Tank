#include <iostream>
#include "NetCore.h"

enum EGameEventCode
{
	GAME_EVENT_CODE_CHAT,

};

struct Chat
{
	UINT32 sender;
	UINT32 length;
	BYTE contents[128];
};

struct ChatEvent
{
	const EGameEventCode evCode = GAME_EVENT_CODE_CHAT;
	Chat chat;
};

NetCore* s_pNetCore;

unsigned WINAPI ReceiveHandler(LPVOID args)
{
	// Receive를 호출해줘야....
	while ( true ) {
		Sleep(10);
		NetMessageQueue* messages = s_pNetCore->GetReceiveMessages();
		UINT32 sender = 0;
		UINT32 msgLength = 0;
		
		NetMessage* pNetMessage = messages->GetNetMessageOrNull(&sender);
		ChatEvent* pChatEvent;
		while ( pNetMessage != NULL ) {
			pChatEvent = (ChatEvent*)pNetMessage->body;
			printf("%.*s\n", pChatEvent->chat.length, pChatEvent->chat.contents);
			pNetMessage = messages->GetNetMessageOrNull(&sender);
		}
	}
	return 0;
}


int main(int argc, char* args[])
{	
	printf("hahahaha\n");
	
	CreateNetCore(&s_pNetCore);
	s_pNetCore->StartNetCore();

	UINT32 serverSessionID = s_pNetCore->ConnectTo("127.0.0.1", NET_CORE_PORT);

	_beginthreadex(NULL, 0, ReceiveHandler, NULL, 0, NULL);

	ChatEvent* pChatEvent = new ChatEvent;
	

	Chat* pChat = &pChatEvent->chat;
	
	while ( true ) {
		pChat->sender = 0;
		fgets((char*)pChat->contents, 128, stdin);
		pChat->length = strlen((char*)pChat->contents);
		s_pNetCore->SendMessageTo(serverSessionID, (BYTE*)pChatEvent, sizeof(ChatEvent));
	}
	
	delete pChatEvent;


	return 0;
}