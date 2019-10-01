#ifndef SESSION
#define SESSION

#include "Predef.h"

struct ICallbackHandler;

class Session
{
public :
	Session();
	~Session();

	BOOL close();

	void		setSocket(uv_tcp_t sock);
	uv_tcp_t*	getSocket();
	BOOL		doReceive();
	BOOL		doSend();

	void		setSessionID(UINT64 id);
	UINT64		getSessionID();
	BOOL		setHandler(ICallbackHandler* handler);

	BOOL		handleRecvPacket(INT32 readsize);
	BOOL		checkPacketHeader(CHAR *buff);

public :
	ICallbackHandler * handler;

	uv_tcp_t		socket;
	uv_connect_t	connectReq;
	uv_write_t		writeReq;
	uv_shutdown_t	shutdownReq;
	uv_async_t		asyncReq;

	// packet ������ ���� ����
	UINT32	dataLen;
	CHAR	recvBuf[RECV_BUF_SIZE];
	CHAR*	buffReadPos;

	// ���� ���� packet�� ó���ϱ� ���� ����
	UINT32	postPacketsize;
	UINT32	postDataLen;
	CHAR	postRecvBuf[RECV_BUF_SIZE];

private:
	UINT32	packetSerial;
	UINT64	sessionID;

	VOID recvDataToHander();
};



#endif