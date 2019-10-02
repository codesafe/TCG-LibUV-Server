
#include "Util.h"
/*

VOID UTIL::Sleep(UINT32 dwMilliseconds)
{
#ifdef OS_WINDOWS
	::Sleep(dwMilliseconds);
#else
	struct timespec req;
	req.tv_sec = 0;
	req.tv_nsec = dwMilliseconds * 1000000;
	nanosleep(&req, NULL);
#endif
	return;
}

tm UTIL::GetCurrTmTime()
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	return *timeinfo;
}

BOOL UTIL::CreateDir(std::string& strDir)
{
	int ret = 0;

#ifdef OS_WINDOWS
	ret = _mkdir(strDir.c_str());
#else
	ret = mkdir(strDir.c_str(), S_IRWXU);
#endif

	if (ret == 0)
		return TRUE;

	if (errno == EEXIST)
		return TRUE;

	return FALSE;
}


UINT32	UTIL::GetCurThreadID()
{
	UINT32 dwThreadID = 0;
#ifdef OS_WINDOWS
	dwThreadID = ::GetCurrentThreadId();
#else
	dwThreadID = (UINT32)pthread_self();
#endif
	return dwThreadID;

}
*/
