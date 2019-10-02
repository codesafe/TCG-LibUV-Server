#include "NetManager.h"
#include "SessionManager.h"
#include "Session.h"
#include "DataBufferPool.h"
#include "../Common/Logger.h"
#include "../Packet.h"

void _Run_Loop(void *arg) 
{
	NetManager *net = (NetManager *)arg;
	net->RunLoop();
	return;
}

void On_Async_Event(uv_async_t* handle)
{
// 	Session *pConnection = (CConnection *)handle->data;
// 
// 	pConnection->DoSend();
// 
// 	uv_close((uv_handle_t*)&pConnection->m_AsyncReq, NULL);

	return;
}

// 서버에서 다른서버로 접속할때 (서버 --> 서버)
void On_Connection(uv_connect_t* req, int status)
{
	Session *session = (Session*)req->data;
	if (status == 0)
		NetManager::instance()->onNewSession(session, status);
 	else
 		session->close();
	return;
}

// Client 접속 발생
void OnNewConnection(uv_stream_t* server, int status)
{
	if (status < 0)
		return;

	Session *session = SessionManager::instance()->createSession();

	if (session == nullptr)
	{
		Log::instance()->LogInfo("LibUV new session is NULL");
		ASSERT_FAIELD;
		return;
	}

	uv_tcp_init(NetManager::instance()->mainLoop, session->getSocket());

	if (uv_accept(server, (uv_stream_t*)session->getSocket()) == 0)
	{
		NetManager::instance()->handleAccept(session, 0);
	}
	else
		session->close();

	return;
}

//////////////////////////////////////////////////////////////////////////


NetManager*	NetManager::_instance = nullptr;

NetManager::NetManager()
{

}

NetManager::~NetManager()
{

}

BOOL NetManager::start(UINT16 port, UINT32 poolnum, ICallbackHandler* handler, std::string &strListenIp)
{
	if (handler == nullptr)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	this->handler = handler;

	//CConnectionMgr::GetInstancePtr()->InitConnectionList(poolnum);

	if (strListenIp.empty() || strListenIp.length() < 4)
		strListenIp = "0.0.0.0";

	mainLoop = uv_default_loop();
	uv_tcp_init(mainLoop, &listenSocket);

	sockaddr_in addr;
	uv_ip4_addr(strListenIp.c_str(), port, &addr);
	uv_tcp_bind(&listenSocket, (const struct sockaddr*)&addr, 0);

	int retCode = uv_listen((uv_stream_t*)&listenSocket, MAX_BACKLOG, OnNewConnection);
	if (retCode)
		return FALSE;

	uv_thread_create(&loopThreadID, _Run_Loop, this);

	return TRUE;
}

BOOL NetManager::close()
{
	uv_stop(mainLoop);
	uv_loop_close(mainLoop);
	uv_thread_join(&loopThreadID);

	//CConnectionMgr::GetInstancePtr()->CloseAllConnection();
	//CConnectionMgr::GetInstancePtr()->DestroyAllConnection();

	return TRUE;
}

BOOL NetManager::sendProtoBufToSession(UINT64 sessionid, UINT32 msgID, CHAR* data, UINT32 len)
{
	Session *session = SessionManager::instance()->getSession(sessionid);
	if (session == nullptr)
		return FALSE;

	if (session->isConnected() == FALSE)
	{
		Log::instance()->LogError("sendBufBySessionID FAILED, disconnected !");
		return FALSE;
	}

	DataBuff * buf = DataBufferManager::instance()->allocBuffer(0);
	PacketHeader *header = (PacketHeader*)buf->buffer;

	header->signature = SERVER_SIG;
	header->msgID = msgID;
	header->packetsize = len + sizeof(PacketHeader);	// protocol buf size + header size

	buf->copyData(data, sizeof(PacketHeader), len);
	if (session->sendBuffer(buf))
	{
		// TODO. Post send buffer

		return TRUE;
	}

	return FALSE;
}

BOOL NetManager::sendBufBySessionID(UINT64 sessionid, DataBuff* buffer)
{
	Session *session = SessionManager::instance()->getSession(sessionid);
	if (session == nullptr)
		return FALSE;

	if (session->isConnected() == FALSE)
	{
		Log::instance()->LogError("sendBufBySessionID FAILED, disconnected !");
		return FALSE;
	}


	return TRUE;
}

Session* NetManager::ConnectTo_Sync(std::string strIpAddr, UINT16 sPort)
{
	return nullptr;
}

Session* NetManager::ConnectTo_Async(std::string strIpAddr, UINT16 sPort)
{
	return nullptr;
}

void NetManager::onNewSession(Session* session, INT32 dwStatus)
{
	handler->onNewSession(session);
	session->doReceive();
}

// 새로운 연결이 들어왔을때 다른 Layer로 전달
void NetManager::handleAccept(Session* session, INT32 status)
{
	if (status == 0)
	{
		handler->onNewSession(session);
		session->setHandler(handler);
		session->doReceive();
	}
	else
	{
		session->close();
	}
}

BOOL NetManager::PostSendOperation(Session* session)
{
	if (session == nullptr)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

// 	if (!pConnection->m_IsSending)
// 	{
		uv_async_init(mainLoop, &session->asyncReq, On_Async_Event);
		session->asyncReq.data = session;
		uv_async_send(&session->asyncReq);
// 	}

	return TRUE;
}

void NetManager::RunLoop()
{
	uv_run(mainLoop, UV_RUN_DEFAULT);
}

