#ifndef SESSION_MANAGER
#define SESSION_MANAGER

#include "Predef.h"

class Session;

class SessionManager
{

public:
	static SessionManager* instance()
	{
		if (_instance == nullptr)
			_instance = new SessionManager();
		return _instance;
	}

	Session*	createSession();
	BOOL		removeSession(Session *session);


private:
	SessionManager();
	~SessionManager();

	static SessionManager*	_instance;

	UINT64					sessionSerial;

	std::map<UINT64, Session*>	sessionlist;
};



#endif
