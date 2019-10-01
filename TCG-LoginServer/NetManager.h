#ifndef NET_MANAGER
#define NET_MANAGER

#include "Predef.h"
#include "IHandler.h"

class Session;

class NetManager
{
public:
	static NetManager * instance()
	{
		if (_instance == nullptr)
			_instance = new NetManager();
		return _instance;
	}

public:
	BOOL	start(UINT16 port, UINT32 connectpool, ICallbackHandler* handler, std::string &strListenIp);
	BOOL	close();

	BOOL	SendMessageByConnID(UINT32 sessionid, UINT32 msgID, UINT64 u64TargetID, UINT32 userData, const char* data, UINT32 len);
	BOOL    SendMsgBufByConnID(UINT32 sessionid, DataBuff* databuffer);

public:
	Session * ConnectTo_Sync(std::string strIpAddr, UINT16 sPort);
	Session*	ConnectTo_Async(std::string strIpAddr, UINT16 sPort);

	void onNewSession(Session* pConnection, INT32 dwStatus);
	void handleAccept(Session* pConnection, INT32 dwStatus);
	BOOL PostSendOperation(Session* pConnection);
	void RunLoop();

	uv_tcp_t						listenSocket;
	uv_loop_t*						mainLoop;
	uv_thread_t						loopThreadID;

public:
	ICallbackHandler * handler;

private:
	static NetManager* _instance;

	NetManager();
	~NetManager();

};

#endif

