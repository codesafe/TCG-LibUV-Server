#ifndef SERVER_BASE
#define SERVER_BASE

#include "Predef.h"
#include "IHandler.h"

class Session;
class DataBuff;

class ServerCore : public ICallbackHandler //public IPacketHandler
{

public:
	static ServerCore * instance()
	{
		if (_instance == nullptr)
			_instance = new ServerCore();
		return _instance;
	}

	BOOL start(UINT16 port, UINT32 connectpool, ICallbackHandler* dispather, std::string ip = "");
	BOOL stop();

	BOOL onRecvData(DataBuff* dataBuffer, Session* session);
	BOOL onCloseSession(Session* session);
	BOOL onNewSession(Session* session);
	BOOL dispatchPacket(NetPacket* packet);

	Session* connectTo(std::string strIpAddr, UINT16 port);

	BOOL	sendMsgProtoBuf(UINT64 sessid, UINT32 msgID, UINT64 targetID, UINT32 userData, const google::protobuf::Message& data);
	BOOL	sendMsgBuffer(UINT64 sessid, DataBuff* dataBuffer);

	Session* getConnectionByID(UINT64 sessid);

	BOOL update();

protected:
	ServerCore();
	~ServerCore();

protected:

	static ServerCore* _instance;

	ICallbackHandler* packetDispatcher;

	std::deque<NetPacket>*				recvDataQueue;
	std::deque<NetPacket>*				dispathQueue;
	CSpinLock							spinLock;

};

#endif

