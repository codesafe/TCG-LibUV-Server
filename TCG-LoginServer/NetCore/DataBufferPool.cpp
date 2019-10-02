#include "DataBufferPool.h"

DataBuff::DataBuff()
{
#ifndef USE_STL
	prev = nullptr;
	next = nullptr;
#endif
}

DataBuff::~DataBuff()
{
}

VOID DataBuff::copyData(CHAR *data, INT32 offset, INT32 size)
{
	memcpy(buffer+offset, data, size);
}

VOID DataBuff::release()
{
	DataBufferManager::instance()->releaseBuffer(this);
}

//////////////////////////////////////////////////////////////////////////

#ifdef USE_STL

DataBufferPool::DataBufferPool()
{
}

DataBufferPool::~DataBufferPool()
{
}

BOOL DataBufferPool::init(int poolnum)
{
	for (int i = 0; i < poolnum; i++)
	{
		DataBuff *buff = new DataBuff();
		bufferpool.push_back(buff);
	}

	return TRUE;
}

// ���� ���������� head���� ��´�
DataBuff *DataBufferPool::allocBuffer()
{
	DataBuff *ret = nullptr;

	bufferlock.lock();

	if (bufferpool.empty() == false)
	{
		ret = bufferpool[0];
		bufferpool.pop_front();
	}
	else
	{
		// ���ڸ��� �� ������ �Ѵ�.
		ret = new DataBuff();
	}

	bufferlock.unlock();

	return ret;
}

// �����ÿ��� head �տ� �߰�
BOOL DataBufferPool::releaseBuffer(DataBuff *buff)
{
	bufferlock.lock();
	bufferpool.push_front(buff);
	bufferlock.unlock();
	return TRUE;
}


#else

DataBufferPool::DataBufferPool()
{
	head = nullptr;
	//tail = nullptr;
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
	//tail = prevbuf;
	return TRUE;
}

// ���� ���������� head���� ��´�
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
		// ���ڸ��� �� ������ �Ѵ�.
		ret = new DataBuff();
	}
	bufferlock.unlock();

	return ret;
}

// �����ÿ��� head �տ� �߰�
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

#endif

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
	// size ���� �غ��ؼ� ����??
	return databufferpool.allocBuffer();
}

BOOL DataBufferManager::releaseBuffer(DataBuff *buff)
{
	return databufferpool.releaseBuffer(buff);
}