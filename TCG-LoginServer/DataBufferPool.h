#ifndef DATABUFFER_POOL
#define DATABUFFER_POOL

#include "Predef.h"
#include "IHandler.h"


class DataBuff //: public IDataBuffer
{
public :
	DataBuff();
	~DataBuff();

	VOID	copyData(CHAR *data, INT32 size);
	VOID	release();

	UINT32		buffersize;
	CHAR		buffer[RECV_BUF_SIZE];

	DataBuff *	prev;
	DataBuff *	next;
};

//////////////////////////////////////////////////////////////////////////

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
	DataBuff * tail;

	std::mutex	bufferlock;
};

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