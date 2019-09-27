#ifndef LOG
#define LOG

#include "Predef.h"

enum LogLevel
{
	Log_All,
	Log_Info,
	Log_Warnning,
	Log_Error,
	Log_None
};


class Log
{
private:
	Log();
	~Log();

public:
	static Log* instance()
	{
		if (_instance == nullptr)
			_instance = new Log();
		return _instance;
	}

	BOOL startLog(std::string strPrefix, std::string strLogDir = "log");
	BOOL closeLog();
	void LogWarnning(char* lpszFormat, ...);
	void LogError(char* lpszFormat, ...);
	void LogInfo(char* lpszFormat, ...);
	void SetLogLevel(int Level);
	//void SetTitle(char* lpszFormat, ...);
	void Flush();

private :
	static Log * _instance;

protected:
	std::mutex			writeMutex;
	INT32				logCount;
	FILE*				logFile;
	INT32				logLevel;
	std::string         strPrefix;
};

#endif