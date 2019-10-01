
#include "Logger.h"
#include "Util.h"

Log * Log::_instance = nullptr;

Log::Log()
{
	logLevel = 0;
}

Log::~Log()
{
	closeLog();
}

BOOL Log::startLog(std::string strPrefix, std::string strLogDir)
{
	if (!UTIL::CreateDir(strLogDir))
		return FALSE;

	this->strPrefix = strPrefix;
#ifdef WIN32
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_EXTENDED_FLAGS);
#endif
	logFile = NULL;
	tm CurTime = UTIL::GetCurrTmTime();
	CHAR szFileName[512];

	snprintf(szFileName, 512, "%s/%s-%02d%02d%02d-%02d%02d%02d.log", strLogDir.c_str(), strPrefix.c_str(), 
		CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec);

	logFile = fopen(szFileName, "w+");

	if (logFile == NULL)
		return FALSE;

	logCount = 0;
	return TRUE;
}

BOOL Log::closeLog()
{
	if (logFile == NULL)
		return FALSE;

	fflush(logFile);
	fclose(logFile);
	logFile = NULL;
	return TRUE;
}

void Log::LogWarnning(char* lpszFormat, ...)
{
	if (logLevel >= Log_Info)
		return;

	if (logFile == NULL)
		return;

	tm CurTime = UTIL::GetCurrTmTime();

	CHAR szLog[512];
	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", strPrefix.c_str(), 
		CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 
		0xffff & UTIL::GetCurThreadID());

	va_list argList;
	va_start(argList, lpszFormat);
	vsnprintf(szLog + 28 + strPrefix.size(), 512 - 28 - strPrefix.size(), lpszFormat, argList);
	va_end(argList);

	strncat(szLog, "\n", 10);

	writeMutex.lock();
	fputs(szLog, logFile);
	logCount++;
	printf(szLog);
	writeMutex.unlock();

	return;
}

void Log::LogError(char* lpszFormat, ...)
{
	if (logLevel >= Log_Error)
		return;

	if (logFile == NULL)
		return;

	CHAR szLog[512];
	tm CurTime = UTIL::GetCurrTmTime();
	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", strPrefix.c_str(), 
		CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 
		0xffff & UTIL::GetCurThreadID());

	va_list argList;
	va_start(argList, lpszFormat);
	vsnprintf(szLog + 28 + strPrefix.size(), 512 - 28 - strPrefix.size(), lpszFormat, argList);
	va_end(argList);

	strncat(szLog, "\n", 10);

	writeMutex.lock();
	fputs(szLog, logFile);
	logCount++;
	printf(szLog);
	writeMutex.unlock();

	return;
}

void Log::LogInfo(char* lpszFormat, ...)
{
	if (logLevel >= Log_Info)
		return;

	if (logFile == NULL)
		return;

	CHAR szLog[512];

	tm CurTime = UTIL::GetCurrTmTime();
	snprintf(szLog, 512, "[%s][%02d-%02d-%02d %02d:%02d:%02d][%04x] ", strPrefix.c_str(), 
		CurTime.tm_year % 100, CurTime.tm_mon + 1, CurTime.tm_mday, CurTime.tm_hour, CurTime.tm_min, CurTime.tm_sec, 
		0xffff & UTIL::GetCurThreadID());

	va_list argList;
	va_start(argList, lpszFormat);
	vsnprintf(szLog + 28 + strPrefix.size(), 512 - 28 - strPrefix.size(), lpszFormat, argList);
	va_end(argList);

	strncat(szLog, "\n", 10);

	writeMutex.lock();
	fputs(szLog, logFile);
	logCount++;
	printf(szLog);
	writeMutex.unlock();

	return;
}

void Log::SetLogLevel(int Level)
{
	logLevel = Level;
	return;
}

/*
void Log::SetTitle(char* lpszFormat, ...)
{
#ifdef OS_WINDOWS
	CHAR szLog[512] = { 0 };
	snprintf(szLog, 512, "%s: ", strPrefix.c_str());

	UINT32 nSize = (UINT32)strlen(szLog);

	va_list argList;
	va_start(argList, lpszFormat);
	vsnprintf(szLog + nSize, 512 - nSize, lpszFormat, argList);
	va_end(argList);

	SetConsoleTitle(szLog);
#endif
	return;
}
*/

void Log::Flush()
{
	if (logCount > 0)
	{
		fflush(logFile);
		logCount = 0;
	}
}
