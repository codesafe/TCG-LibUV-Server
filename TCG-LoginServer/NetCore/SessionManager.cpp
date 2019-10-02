
#include "SessionManager.h"
#include "Session.h"
#include "../Common/Logger.h"

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

	lockobj.lock();
	sessionlist.insert(std::make_pair(newsession->getSessionID(), newsession));
	lockobj.unlock();

	return newsession;
}


BOOL SessionManager::removeSession(Session *session)
{
	lockobj.lock();
	std::map<UINT64, Session*>::iterator it = sessionlist.find(session->getSessionID());
	if (it == sessionlist.end())
	{
		Log::instance()->LogError("<SessionManager::removeSession> Not found session id : %d", session->getSessionID());
		lockobj.unlock();
		return FALSE;
	}

	sessionlist.erase(it);
	lockobj.unlock();

	Log::instance()->LogInfo("Removed session id : %d", session->getSessionID());
	delete session;

	return TRUE;
}

Session* SessionManager::getSession(UINT64 sessionid)
{
	Session* retsession = nullptr;

	lockobj.lock();
	std::map<UINT64, Session*>::iterator it = sessionlist.find(sessionid);
	if (it != sessionlist.end())
		retsession = it->second;
	lockobj.unlock();

	return retsession;
}