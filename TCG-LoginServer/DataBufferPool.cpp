#include "DataBufferPool.h"

DataBuff::DataBuff()
{
	prev = nullptr;
	next = nullptr;
}

DataBuff::~DataBuff()
{

}

VOID DataBuff::copyData(CHAR *data, INT32 size)
{
	memcpy(buffer, data, size);
}

VOID	DataBuff::release()
{
	DataBufferManager::instance()->releaseBuffer(this);
}

//////////////////////////////////////////////////////////////////////////

DataBufferPool::DataBufferPool()
{
	head = nullptr;
	tail = nullptr;
}

DataBufferPool::~DataBufferPool()
{

}


BOOL DataBufferPool::init(int poolnum)
{
	DataBuff *prevbuf = nullptr;
	for (int i = 0; i < poolnum; i++)
	{
		DataBuff *buff = new DataBuff();
		if (head == nullptr)
			head = buff;
		else
		{
			prevbuf->next = buff;
			buff->prev = prevbuf;
		}

		prevbuf = buff;
	}
	tail = prevbuf;
	return TRUE;
}

DataBuff *DataBufferPool::allocBuffer()
{
	DataBuff *ret = nullptr;

	bufferlock.lock();
	if (head != nullptr)
	{
		ret = head;
		head = head->next;
		head->prev = nullptr;

		ret->next = nullptr;
		ret->prev = nullptr;
	}
	else
	{
		// 모자르다 더 만들어야 한다.
		ret = new DataBuff();
	}
	bufferlock.unlock();

	return ret;
}

BOOL DataBufferPool::releaseBuffer(DataBuff *buff)
{
	bufferlock.lock();
	if (head == nullptr)
	{
		head = buff;
	}
	else
	{
		head->prev = buff;
		buff->next = head;
		head = buff;
	}
	bufferlock.unlock();
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////

DataBufferManager * DataBufferManager::_instance = nullptr;

DataBufferManager::DataBufferManager()
{

}

DataBufferManager::~DataBufferManager()
{

}

DataBuff* DataBufferManager::allocBuffer(int size)
{
	// size 별로 준비해서 꺼내??
	return databufferpool.allocBuffer();
}

BOOL DataBufferManager::releaseBuffer(DataBuff *buff)
{
	return databufferpool.releaseBuffer(buff);
}