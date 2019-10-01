#include "ServerCore.h"
#include "NetManager.h"
#include "Session.h"
#include "SessionManager.h"
#include "Logger.h"
#include "Packet.h"
#include "DataBufferPool.h"


ServerCore*	ServerCore::_instance = nullptr;

ServerCore::ServerCore()
{
	recvDataQueue = new std::deque<NetPacket>();
	dispathQueue = new std::deque<NetPacket>();
}

ServerCore::~ServerCore()
{
	delete recvDataQueue;
	delete dispathQueue;
}

BOOL ServerCore::start(UINT16 port, UINT32 connectpool, ICallbackHandler* dispather, std::string strListenIp)
{
	if (dispather == nullptr)
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	if ((port == 0) || (connectpool == 0))
	{
		ASSERT_FAIELD;
		return FALSE;
	}

	packetDispatcher = dispather;

	if (!NetManager::instance()->start(port, connectpool, this, strListenIp))
	{
		Log::instance()->LogError("Error on Network init");
		return FALSE;
	}

	Log::instance()->LogInfo("Started LibUV Server");

	return TRUE;
}

BOOL ServerCore::stop()
{
	return NetManager::instance()->close();
}


Session* ServerCore::connectTo(std::string strIpAddr, UINT16 sPort)
{
	return nullptr;
}

// 해당 세션에게 protbuf Packet 전송
BOOL ServerCore::sendMsgProtoBuf(UINT64 sessid, UINT32 msgID, UINT64 targetID, UINT32 userData, const google::protobuf::Message& data)
{

	return TRUE;
}

// 해당 세션에게 buf Packet 전송
BOOL	ServerCore::sendMsgBuffer(UINT64 sessid, DataBuff* dataBuffer)
{
	return TRUE;
}


Session* ServerCore::getConnectionByID(UINT64 sessid)
{
	return nullptr;
}

BOOL ServerCore::update()
{
	//CConnectionMgr::GetInstancePtr()->CheckConntionAvalible();

	spinLock.Lock();
	std::swap(recvDataQueue, dispathQueue);
	spinLock.Unlock();

	for (std::deque<NetPacket>::iterator itor = dispathQueue->begin(); itor != dispathQueue->end(); itor++)
	{
		NetPacket& item = *itor;
		if (item.msgID == MSG_NEW_CONNECTION)
		{
			packetDispatcher->onNewSession((Session*)item.dataBuffer);
		}
		else if (item.msgID == MSG_CLOSE_CONNECTION)
		{
			packetDispatcher->onCloseSession((Session*)item.dataBuffer);

			// 세션 메니저에서 세션 삭제
			SessionManager::instance()->removeSession((Session*)item.dataBuffer);
		}
		else
		{
			// 여기에서 처리하면될껄 dispatcher를 불러서 처리해야하는가??
			packetDispatcher->dispatchPacket(&item);
 			item.dataBuffer->release();
		}
	}

	dispathQueue->clear();
	Log::instance()->Flush();

	return TRUE;
}

BOOL ServerCore::onRecvData(DataBuff* dataBuffer, Session* session)
{
	PacketHeader* header = (PacketHeader*)dataBuffer->buffer;

	spinLock.Lock();
	recvDataQueue->push_back(NetPacket(session->getSessionID(), dataBuffer, header->msgID));
	//recvDataQueue->emplace_back(NetPacket(session->getSessionID(), pDataBuffer, header->msgID));
	spinLock.Unlock();
	return TRUE;
}

BOOL ServerCore::onCloseSession(Session* session)
{
	spinLock.Lock();
	recvDataQueue->emplace_back(NetPacket(session->getSessionID(), (DataBuff*)session, MSG_CLOSE_CONNECTION));
	spinLock.Unlock();

	return TRUE;
}

BOOL ServerCore::onNewSession(Session* session)
{
	spinLock.Lock();
	recvDataQueue->emplace_back(NetPacket(session->getSessionID(), (DataBuff*)session, MSG_NEW_CONNECTION));
	spinLock.Unlock();

	return TRUE;
}

BOOL ServerCore::dispatchPacket(NetPacket* packet)
{
	ASSERT_FAIELD;
	return FALSE;
}