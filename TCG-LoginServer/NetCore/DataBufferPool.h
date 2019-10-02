#ifndef DATABUFFER_POOL
#define DATABUFFER_POOL

#include "../Predef.h"
#include "IHandler.h"

#define USE_STL

class DataBuff //: public IDataBuffer
{
public :
	DataBuff();
	~DataBuff();

	VOID	copyData(CHAR *data, INT32 offset, INT32 size);
	VOID	release();

	UINT32		buffersize;
	CHAR		buffer[RECV_BUF_SIZE];

#ifndef USE_STL
	DataBuff *	prev;
	DataBuff *	next;
#endif
};

//////////////////////////////////////////////////////////////////////////


#ifdef USE_STL

class DataBufferPool
{
public:
	DataBufferPool();
	~DataBufferPool();

	BOOL		init(int poolnum);
	DataBuff*	allocBuffer();
	BOOL		releaseBuffer(DataBuff *buff);

private:
	std::deque<DataBuff *>	bufferpool;
	std::mutex	bufferlock;
};


#else

class DataBufferPool
{
public :
	DataBufferPool();
	~DataBufferPool();

	BOOL		init(int poolnum);
	DataBuff*	allocBuffer();
	BOOL		releaseBuffer(DataBuff *buff);

private :
	DataBuff * head;
	//DataBuff * tail;

	std::mutex	bufferlock;
};

#endif

//////////////////////////////////////////////////////////////////////////

// size 별로 준비??

class DataBufferManager
{
private :
	DataBufferManager();
	~DataBufferManager();

	static DataBufferManager * _instance;

	DataBufferPool	databufferpool;

public :

	static DataBufferManager * instance()
	{
		if (_instance == nullptr)
			_instance = new DataBufferManager();
		return _instance;
	}

	DataBuff* allocBuffer(int size);
	BOOL	releaseBuffer(DataBuff *buff);
};


#endif