#ifndef GAME_SERVICE
#define GAME_SERVICE

#include "./NetCore/IHandler.h"
#include "PacketHandler.h"

struct NetPacket;
class Session;

class GameService : public ICallbackHandler//public IPacketDispatcher
{
public :
	GameService();
	~GameService();

public :
	bool	init();
	bool	uninit();

	bool	runLoop();

	BOOL	dispatchPacket(NetPacket* packet);
	BOOL	onCloseSession(Session* session);
	BOOL	onNewSession(Session* session);
	BOOL	onRecvData(DataBuff* dataBuffer, Session* session);

	BOOL	onSecondTimer();
private :
	PacketHandler	packethandler;

};



#endif


