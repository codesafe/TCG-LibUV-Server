#include "ServerCore.h"
#include "NetManager.h"
#include "Session.h"
#include "Logger.h"

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

BOOL ServerCore::start(UINT16 port, UINT32 maxconn, IPacketDispatcher* dispather, std::string strListenIp)
{
// 	if (pDispather == NULL)
// 	{
// 		ASSERT_FAIELD;
// 		return FALSE;
// 	}

	if ((port == 0) || (maxconn == 0))
	{
		assert(0);
		return FALSE;
	}

	packetDispatcher = dispather;

	if (!NetManager::instance()->start(port, maxconn, this, strListenIp))
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

BOOL ServerCore::onDataHandle(IDataBuffer* pDataBuffer, Session* session)
{
	return TRUE;
}

BOOL ServerCore::onCloseConnect(Session* session)
{
	return TRUE;
}

BOOL ServerCore::onNewConnect(Session* session)
{
	return TRUE;
}

Session* ServerCore::connectTo(std::string strIpAddr, UINT16 sPort)
{
	return nullptr;
}

// BOOL ServerBase::sendMsgProtoBuf(UINT32 dwConnID, UINT32 dwMsgID, UINT64 u64TargetID, UINT32 dwUserData, const google::protobuf::Message& pdata)
// {
// 
// }

Session* ServerCore::getConnectionByID(UINT32 dwConnID)
{
	return nullptr;
}

BOOL ServerCore::update()
{
	//CConnectionMgr::GetInstancePtr()->CheckConntionAvalible();

	spinLock.Lock();
	std::swap(recvDataQueue, dispathQueue);
	spinLock.Unlock();

// 	for (std::deque<NetPacket>::iterator itor = dispathQueue->begin(); itor != dispathQueue->end(); itor++)
// 	{
// 		NetPacket& item = *itor;
// 		if (item.m_dwMsgID == NEW_CONNECTION)
// 		{
// 			packetDispatcher->OnNewConnect((CConnection*)item.m_pDataBuffer);
// 		}
// 		else if (item.m_dwMsgID == CLOSE_CONNECTION)
// 		{
// 			packetDispatcher->OnCloseConnect((CConnection*)item.m_pDataBuffer);
// 			//알림 보내기
// 			CConnectionMgr::GetInstancePtr()->DeleteConnection((CConnection*)item.m_pDataBuffer);
// 		}
// 		else
// 		{
// 			m_pPacketDispatcher->DispatchPacket(&item);
// 			item.m_pDataBuffer->Release();
// 		}
// 	}

	dispathQueue->clear();
	Log::instance()->Flush();

	return TRUE;
}