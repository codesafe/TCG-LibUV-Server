#ifndef HANDLER
#define HANDLER

#include "Predef.h"

class Session;

class IDataBuffer
{
public:
	virtual INT32	GetRef() = 0;
	virtual BOOL	AddRef() = 0;
	virtual BOOL	Release() = 0;
	virtual CHAR*	GetData() = 0;
	virtual UINT32	GetTotalLenth() = 0;
	virtual VOID	SetTotalLenth(UINT32 nPos) = 0;
	virtual UINT32  GetBodyLenth() = 0;
	virtual CHAR*	GetBuffer() = 0;
	virtual UINT32	GetBufferSize() = 0;
	virtual UINT32  CopyFrom(IDataBuffer* pSrcBuffer) = 0;
	virtual UINT32  CopyTo(CHAR* pDestBuf, UINT32 dwDestLen) = 0;
};

struct NetPacket
{
	NetPacket(UINT32 _connID = 0, IDataBuffer* _buffer = NULL, UINT32 _msgID = 0)
	{
		connID = _connID;
		dataBuffer = _buffer;
		msgID = _msgID;
	}

	UINT32       msgID;
	UINT32       connID;
	IDataBuffer* dataBuffer;
};

// ¼ö½ÅµÈ packet hadle func callback
struct IPacketHandler
{
	virtual BOOL onDataHandle(IDataBuffer* pDataBuffer, Session* session) = 0;
	virtual BOOL onCloseConnect(Session* session) = 0;
	virtual BOOL onNewConnect(Session* session) = 0;
};

struct IPacketDispatcher
{
	virtual BOOL dispatchPacket(NetPacket* pNetPacket) = 0;
	virtual BOOL onSecondTimer() = 0;
	virtual BOOL onCloseConnect(Session* session) = 0;
	virtual BOOL onNewConnect(Session* session) = 0;
};



#endif
