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

	Session* createSession();


private:
	SessionManager();
	~SessionManager();

	static SessionManager*	_instance;


	std::vector<Session*>	sessionlist;

};



#endif
