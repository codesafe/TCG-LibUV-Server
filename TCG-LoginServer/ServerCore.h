#ifndef SERVER_BASE
#define SERVER_BASE

#include "Predef.h"
#include "IHandler.h"

class Session;

class ServerCore : public IDataHandler
{

public:
	static ServerCore * instance()
	{
		if (_instance == nullptr)
			_instance = new ServerCore();
		return _instance;
	}

	BOOL start(UINT16 nPortNum, UINT32 nMaxConn, IPacketDispatcher* pDispather, std::string strListenIp = "");
	BOOL stop();

	BOOL onDataHandle(IDataBuffer* pDataBuffer, Session* session);
	BOOL onCloseConnect(Session* session);
	BOOL onNewConnect(Session* session);

	Session* connectTo(std::string strIpAddr, UINT16 sPort);

	//BOOL			sendMsgProtoBuf(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata);
	//BOOL			SendMsgBuffer(UINT32 dwConnID, IDataBuffer* pDataBuffer);

	Session* getConnectionByID(UINT32 dwConnID);

	BOOL update();

protected:
	ServerCore();
	~ServerCore();

protected:

	static ServerCore * _instance;

	IPacketDispatcher * packetDispatcher;

	std::deque<NetPacket>*				recvDataQueue;
	std::deque<NetPacket>*				dispathQueue;
	CSpinLock							spinLock;

};

#endif

