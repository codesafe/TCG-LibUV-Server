
#include "SessionManager.h"
#include "Session.h"

SessionManager*	SessionManager::_instance = nullptr;

SessionManager::SessionManager()
{
	
}

SessionManager::~SessionManager()
{

}


Session* SessionManager::createSession()
{
	Session *newsession = new Session();
	sessionlist.push_back(newsession);

	return newsession;
}