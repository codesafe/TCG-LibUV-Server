#ifndef SESSION_MANAGER
#define SESSION_MANAGER

#include "../Predef.h"

class Session;

/*
	TODO. session pool을 만들어 사용하는것도 생각해봐야 한다.
	pool size에 서버의 최대 동접수를 사용한다.
*/

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
	Session*	getSession(UINT64 sessionid);

private:
	SessionManager();
	~SessionManager();

	static SessionManager*	_instance;

	UINT64					sessionSerial;

	std::map<UINT64, Session*>	sessionlist;
	std::mutex	lockobj;
};



#endif
