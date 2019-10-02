#ifndef SESSION
#define SESSION

#include "../Predef.h"
#include "../Common/LockFreeQueue.h"

struct ICallbackHandler;
class DataBuff;


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

	BOOL		sendBuffer(DataBuff *buff);
	BOOL		isConnected();

public :
	ICallbackHandler * handler;

	uv_tcp_t		socket;
	uv_connect_t	connectReq;
	uv_write_t		writeReq;
	uv_shutdown_t	shutdownReq;
	uv_async_t		asyncReq;

	// packet 수신을 위한 버퍼
	UINT32	recvDataLen;
	CHAR	recvBuf[RECV_BUF_SIZE];
	CHAR*	buffReadPos;

	// 실제 받은 packet을 처리하기 위한 버퍼
	UINT32	postPacketsize;
	UINT32	postDataLen;
	CHAR	postRecvBuf[RECV_BUF_SIZE];

	UINT32	sendDataLen;
	CHAR	sendBuf[SEND_BUF_SIZE];

	UINT32		prevSendOffset;
	DataBuff*	prevSendBuf;

private:
	UINT32	packetSerial;
	UINT64	sessionID;
	BOOL	connected;
	BOOL	isSending;

	ArrayLockFreeQueue <DataBuff*> sendBuffList;

	VOID recvDataToHander();
};



#endif