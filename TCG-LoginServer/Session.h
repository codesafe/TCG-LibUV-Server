#ifndef SESSION
#define SESSION

#include "Predef.h"

#define RECV_BUF_SIZE	4096 * 2
struct IDataHandler;

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
	BOOL		setDataHandler(IDataHandler* pHandler);

	BOOL		handleRecvPacket(INT32 readsize);
	BOOL		checkPacketHeader(CHAR *buff);

public :
	uv_tcp_t		socket;
	uv_connect_t	connectReq;
	uv_write_t		writeReq;
	uv_shutdown_t	shutdownReq;
	uv_async_t		asyncReq;

	// packet 수신을 위한 버퍼
	UINT32	dataLen;
	CHAR	recvBuf[RECV_BUF_SIZE];
	CHAR*	buffReadPos;

	// 실제 받은 packet을 처리하기 위한 버퍼
	UINT32	postPacketsize;
	UINT32	postDataLen;
	CHAR	postRecvBuf[RECV_BUF_SIZE];

	UINT64	sessionID;
	IDataHandler*	dataHandler;
};



#endif