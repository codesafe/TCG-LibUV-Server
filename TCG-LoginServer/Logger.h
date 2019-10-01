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

#define ASSERT_FAIELD (assert(false))
#undef ASSERT
#define ASSERT(x)     (assert(x))

#define LOG_ERROR  Log::instance()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);

#define ERROR_RETURN_TRUE(P) \
    if(P == FALSE)\
    {\
        Log::instance()->LogError("Error : File:%s, Func: %s Line:%d", __FILE__ , __FUNCTION__, __LINE__);\
        ASSERT_FAIELD; \
        return TRUE;	\
    }


#endif