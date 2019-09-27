#ifndef UTIL
#define UTIL

#include "Predef.h"

namespace UTIL
{
	static VOID	Sleep(UINT32 dwMilliseconds)
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

	static tm		GetCurrTmTime()
	{
		time_t rawtime;
		struct tm* timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		return *timeinfo;
	}
	static BOOL	CreateDir(std::string& strDir)
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

	static UINT32	GetCurThreadID()
	{
		UINT32 dwThreadID = 0;
#ifdef OS_WINDOWS
		dwThreadID = ::GetCurrentThreadId();
#else
		dwThreadID = (UINT32)pthread_self();
#endif
		return dwThreadID;
	}

#if 0
	static std::string GetCurrentExeDir()
	{
		wchar szPath[1024] = { 0 };
#ifdef OS_WINDOWS
		GetModuleFileName(NULL, szPath, 1024);
		char* p = wcsrchr(szPath, '\\');
#else
		readlink("/proc/self/exe", szPath, sizeof(szPath));
		char* p = strrchr(szPath, '/');
#endif
		*p = 0;
		return std::string(szPath);
	}

	static BOOL SetCurrentWorkDir(std::string strPath)
	{
		if (strPath.empty())
			strPath = GetCurrentExeDir();

#ifdef OS_WINDOWS
		SetCurrentDirectory(strPath.c_str());
#else
		chdir(strPath.c_str());
#endif
		return TRUE;
	}

#endif
}

#endif