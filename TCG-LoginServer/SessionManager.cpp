#include "Logger.h"
#include "SessionManager.h"
#include "Session.h"

SessionManager*	SessionManager::_instance = nullptr;

SessionManager::SessionManager()
{
	sessionSerial = 0;
}

SessionManager::~SessionManager()
{

}


Session* SessionManager::createSession()
{
	Session *newsession = new Session();

	Log::instance()->LogInfo("New Session %d created", sessionSerial);
	newsession->setSessionID(sessionSerial++);
	sessionlist.insert(std::make_pair(newsession->getSessionID(), newsession));

	return newsession;
}


BOOL SessionManager::removeSession(Session *session)
{
	std::map<UINT64, Session*>::iterator it = sessionlist.find(session->getSessionID());

	if (it == sessionlist.end())
	{
		Log::instance()->LogError("<SessionManager::removeSession> Not found session id : %d", session->getSessionID());
		return FALSE;
	}

	Log::instance()->LogInfo("Removed session id : %d", session->getSessionID());
	sessionlist.erase(it);

	delete session;

	return TRUE;
}