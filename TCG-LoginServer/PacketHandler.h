#ifndef PACKET_HANDLER
#define PACKET_HANDLER

#include "Predef.h"
#include "IHandler.h"


class PacketHandler
{
public :
	PacketHandler();
	~PacketHandler();

	BOOL dispatchPacket(NetPacket* pNetPacket);

	BOOL onCreateAccountReq(NetPacket* packet);
	BOOL onLoginReq(NetPacket* pPacket);


private:

};


#endif
