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
	BOOL	start(UINT16 nPortNum, UINT32 nMaxConn, IDataHandler* pBufferHandler, std::string &strListenIp);
	BOOL	close();

	BOOL	SendMessageByConnID(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const char* pData, UINT32 dwLen);
	BOOL    SendMsgBufByConnID(UINT32 dwConnID, IDataBuffer* pBuffer);

public:
	Session * ConnectTo_Sync(std::string strIpAddr, UINT16 sPort);

	Session*	ConnectTo_Async(std::string strIpAddr, UINT16 sPort);

	void HandleConnect(Session* pConnection, INT32 dwStatus);

	void forwardNewConnect(Session* pConnection, INT32 dwStatus);

	BOOL PostSendOperation(Session* pConnection);

	void RunLoop();

	uv_tcp_t						listenSocket;
	uv_loop_t*						mainLoop;
	uv_thread_t						loopThreadID;

public:
	IDataHandler * bufferHandler;

private:
	static NetManager* _instance;

	NetManager();
	~NetManager();

};

#endif

