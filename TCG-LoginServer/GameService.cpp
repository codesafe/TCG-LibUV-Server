#include "GameService.h"
#include "Util.h"
#include "ServerCore.h"
#include "Logger.h"

GameService::GameService()
{
}

GameService::~GameService()
{
}

bool GameService::init()
{

//	std::string key = UTIL::GetSessionKey("test124");

	//UTIL::SetCurrentWorkDir("");

	if (!Log::instance()->startLog("LoginServer", "log"))
		return false;

	Log::instance()->LogInfo("Start Game Service");

	int port = 9999;
	int connectpool = 100;
	ServerCore::instance()->start(port, connectpool, this);

	return true;
}

bool GameService::uninit()
{
	Log::instance()->LogInfo("Stop Game Service");
	ServerCore::instance()->stop();

	return true;
}

bool GameService::runLoop()
{
	while (1)
	{
		ServerCore::instance()->update();
		UTIL::Sleep(1);
	}

	return true;
}

// new connection이 왔을때 serverbase로 부터 뭔가 받고 싶으면 이것을 사용
BOOL GameService::onNewSession(Session* session)
{

	return true;
}

// close connection이 왔을때 serverbase로 부터 뭔가 받고 싶으면 이것을 사용
BOOL GameService::onCloseSession(Session* session)
{
	return true;
}

// client로 부터온 모든 패킷 처리
BOOL GameService::dispatchPacket(NetPacket* packet)
{
	// packet 처리
	packethandler.dispatchPacket(packet);

	return TRUE;
}

BOOL GameService::onRecvData(DataBuff* dataBuffer, Session* session)
{
	ASSERT_FAIELD;
	return FALSE;
}

BOOL GameService::onSecondTimer()
{

	return TRUE;
}

