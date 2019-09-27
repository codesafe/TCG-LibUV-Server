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


bool	GameService::init()
{
	//UTIL::SetCurrentWorkDir("");

	if (!Log::instance()->startLog("LoginServer", "log"))
		return false;

	Log::instance()->LogInfo("Start Game Service");

	int port = 9999;
	int maxconnect = 100;
	ServerCore::instance()->start(port, maxconnect, nullptr);

	return true;
}

bool	GameService::uninit()
{
	Log::instance()->LogInfo("Stop Game Service");
	ServerCore::instance()->stop();

	return true;
}

bool	GameService::runLoop()
{
	while (1)
	{
		ServerCore::instance()->update();
		UTIL::Sleep(1);
	}

	return true;
}

// new connection�� ������ serverbase�� ���� ���� �ް� ������ �̰��� ���
bool	GameService::onNewConnection()
{
	return true;
}

// close connection�� ������ serverbase�� ���� ���� �ް� ������ �̰��� ���
bool	GameService::onCloseConnection()
{
	return true;
}