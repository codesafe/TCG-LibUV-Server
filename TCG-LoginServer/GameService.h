#ifndef GAME_SERVICE
#define GAME_SERVICE

#include "IHandler.h"

struct NetPacket;
class Session;


class GameService : public IPacketDispatcher
{
public :
	GameService();
	~GameService();

public :
	bool	init();
	bool	uninit();

	bool	runLoop();

	BOOL	dispatchPacket(NetPacket* pNetPacket);
	BOOL	onSecondTimer();
	BOOL	onCloseConnect(Session* session);
	BOOL	onNewConnect(Session* session);

};



#endif


