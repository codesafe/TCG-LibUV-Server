
#include "Session.h"
#include "IHandler.h"
#include "DataBufferPool.h"
#include "../Common/Logger.h"
#include "../Packet.h"



// libuv에게 packet을 Read할 buffer를 알려준다. 이곳이 call된 후에 자동으로 libuv에서는 packet을 buf에 채운다.
void On_AllocBuff(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	Session* session = (Session*)handle->data;
	buf->base = session->recvBuf + session->recvDataLen;
	buf->len = RECV_BUF_SIZE - session->recvDataLen;
	return;
}

// Packet을 받고난 후의 결과 (이곳이 call되면 이미 buf에 packet을 받은후이다)
void On_ResultReadData(uv_stream_t* stream, ssize_t readsize, const uv_buf_t* buf)
{
	Session* session = (Session*)stream->data;
	assert(session);

	if (readsize >= 0)
	{
		Log::instance()->LogInfo("LibUV Recv Data %d : %s : %d byte", session->getSessionID(), buf->base, readsize);
		// Recv후의 처리
		session->handleRecvPacket((INT32)readsize);
	}
	else
	{
		// read가 -라서 접속이 끊김
		//uv_last_error(uv_default_loop());
		Log::instance()->LogInfo("LibUV Recv Data is minus : %d", session->getSessionID());
		session->close();
	}
	return;
}

void On_WriteData(uv_write_t* req, int status)
{
	Session* session = (Session*)req->data;
	//ERROR_RETURN_NONE(session != nullptr);
	session->doSend();
	if (status == 0)
	{
		//성공
	}
	else
		Log::instance()->LogError("LibUV On_WriteData Fail.  id : %d, %d", session->getSessionID(), status);

	return;
}

void On_Shutdown(uv_shutdown_t* req, int status)
{
	Session* session = (Session*)req->data;

	Log::instance()->LogInfo("LibUV Shutdown .  id : %d", session->getSessionID());
	//ERROR_RETURN_NONE(pConnection != NULL);

	if (status == 0)
	{
		// 성공
	}
	else
		Log::instance()->LogError("LibUV On_Shutdown Fail.  id : %d, %d", session->getSessionID(), status);

	return;
}

void On_Close(uv_handle_t* handle)
{
	Session* session = (Session*)handle->data;
	Log::instance()->LogInfo("LibUV Close Connection.  id : %d", session->getSessionID());

	//ERROR_RETURN_NONE(pConnection != NULL);
	return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Session::Session()
{
	handler = nullptr;
	sessionID = 0;

	packetSerial = 0;
	connected = FALSE;
	isSending = FALSE;

	socket.data = (void*)this;

	// recv
	recvDataLen = 0;
	memset(recvBuf, 0, RECV_BUF_SIZE);
	buffReadPos = recvBuf;

	postDataLen = 0;
	postPacketsize = 0;
	memset(postRecvBuf, 0, RECV_BUF_SIZE);

	// send
	sendDataLen = 0;
	memset(sendBuf, 0, SEND_BUF_SIZE);
	
	prevSendOffset = 0;
	prevSendBuf = nullptr;
}

Session::~Session()
{
}

BOOL Session::close()
{
	shutdownReq.data = (void*)this;
	uv_read_stop((uv_stream_t*)&socket);
	uv_shutdown(&shutdownReq, (uv_stream_t*)&socket, On_Shutdown);
	uv_close((uv_handle_t*)&socket, On_Close);
	
	recvDataLen = 0;
	isSending = FALSE;

	if (handler != nullptr)
		handler->onCloseSession(this);

	connected = FALSE;

	return TRUE;
}

void Session::setSocket(uv_tcp_t sock)
{
	socket = sock;
}

uv_tcp_t*  Session::getSocket()
{
	return &socket;
}

BOOL Session::doReceive()
{
	if (0 != uv_read_start((uv_stream_t*)&socket, On_AllocBuff, On_ResultReadData))
	{
		ASSERT_FAIELD;
		return FALSE;
	}
	return TRUE;
}


// sendBuf에 보낼것을 꽉채워서 보낸다.
BOOL Session::doSend()
{
	isSending = TRUE;

	DataBuff *currentbuff = nullptr;
	UINT32 bufferlen = 0;

	sendDataLen = 0;

	// 이전에 보내다가 남은것이 있으면 그것을 보내려고 sendBuf에 복사
	if (prevSendBuf != nullptr)
	{
		UINT32 _len = prevSendBuf->buffersize - prevSendOffset;
		memcpy(sendBuf, prevSendBuf + prevSendOffset, _len);
		sendDataLen = _len;

		prevSendBuf->release();
		prevSendBuf = nullptr;
		prevSendOffset = 0;
	}

	// 보내야 할것이 있으면 pop하여 보낸다
	while (sendBuffList.pop(currentbuff))
	{
		bufferlen = currentbuff->buffersize;

		if (sendDataLen + bufferlen >= SEND_BUF_SIZE)
		{
			// 모으다보니 버퍼가 넘친다. 나머지는 다음에 보낸다.
			UINT32 _len = SEND_BUF_SIZE - sendDataLen;
			memcpy(sendBuf + sendDataLen, currentbuff, _len);
			sendDataLen += _len;

			prevSendBuf = currentbuff;
			prevSendOffset = _len;
			break;
		}
		else
		{
			memcpy(sendBuf + sendDataLen, currentbuff, bufferlen);
			sendDataLen += bufferlen;

			currentbuff->release();
			currentbuff = nullptr;

			// 꽉찼으면 보낸다
			if( sendDataLen >= SEND_BUF_SIZE )
				break;
		}
	}

	// 더이상 보낼것이 없으면 return FALSE
	if (sendDataLen == 0)
		return FALSE;

	writeReq.data = (void*)this;
	uv_buf_t buf = uv_buf_init(sendBuf, sendDataLen);
	uv_write(&writeReq, (uv_stream_t*)&socket, &buf, 1, On_WriteData);

	return TRUE;
}

void	Session::setSessionID(UINT64 id)
{
	sessionID = id;
}

UINT64	Session::getSessionID()
{
	return sessionID;
}


BOOL Session::setHandler(ICallbackHandler* handler)
{
	//ERROR_RETURN_FALSE(pHandler != NULL);

	this->handler = handler;

	return TRUE;
}

BOOL Session::handleRecvPacket(INT32 readsize)
{
	while (1)
	{
		// 이전에 받은 것이 있어서 이어서 받는다
		if (postPacketsize > 0)
		{
			if (postDataLen + recvDataLen < postPacketsize)
			{
				// 이어서 받지만 아직 부족하다.
				memcpy(postRecvBuf + postDataLen, buffReadPos, recvDataLen);
				postDataLen += recvDataLen;
				buffReadPos = recvBuf;
				recvDataLen = 0;
				break;
			}
			else
			{
				// 이어서 받다보니 packet 크기를 충족시켰다.
				INT32 _readsize = postPacketsize - postDataLen;
				memcpy(postRecvBuf + postDataLen, buffReadPos, _readsize);

				recvDataLen -= _readsize;
				buffReadPos += _readsize;
				postDataLen += _readsize;

				// 패킷 처리 핸들러에게 전달
				recvDataToHander();
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// 헤더 보다 받은것이 작으면 더받아야 하기때문에 끝
		if(recvDataLen < sizeof(PacketHeader))
			break;

		PacketHeader* header = (PacketHeader*)buffReadPos;

		// 헤더가 유효하지 않은 경우 FALSE
		if (checkPacketHeader(buffReadPos) == FALSE)
			return FALSE;
		
		UINT32 packetSize = header->packetsize;

		// 아직 덜받았다 (패킷의 길이보다 받은 버퍼가 작다)
		if ((packetSize> recvDataLen) && (packetSize < RECV_BUF_SIZE))
			break;

		if (packetSize <= recvDataLen)
		{
			// 패킷을 충분히 받음
			memcpy(postRecvBuf, buffReadPos, packetSize);
			postDataLen = packetSize;

			recvDataLen -= packetSize;
			buffReadPos += packetSize;

			// 패킷 처리 핸들러에게 전달
			recvDataToHander();
		}
		else
		{
			// 아직 패킷을 덜 받았다, 받은 만큼을 일단 보관하고 나머지 수신인 이 loop 위에서 처리
			memcpy(postRecvBuf, buffReadPos, recvDataLen);
			postDataLen = recvDataLen;

			recvDataLen = 0;
			buffReadPos = recvBuf;
			postPacketsize = packetSize;
		}
	}

	return TRUE;
}

// 패킷 처리 핸들러에게 전달
VOID Session::recvDataToHander()
{
	DataBuff * databuff = DataBufferManager::instance()->allocBuffer(100);
	databuff->copyData(postRecvBuf, 0, postDataLen);
	handler->onRecvData(databuff, this);
	postPacketsize = 0;
	postDataLen = 0;
}

// 패킷의 헤더를 검증
BOOL Session::checkPacketHeader(CHAR *buff)
{
/*
	검사 항목
	Packet 고유코드 (서버용/클라이언트용)
	Packet 길이
	Packet 일련번호 (연속적이지 않거나, 중복여부)
	Packet 암호화 여부

	검사 항목에서 하나라도 잘못되면 해당 연결을 무효화(Close) 한다.
	패킷을 변조하여 Server를 공격하거나 할수 있기 때문에 이 부분은 중요
	* 패킷의 길이가 너무 길거나, 길이가 안맞는경우, 길이는 같으나 나머지 내용이 쓰레기 데이터도 포함
*/
	PacketHeader *header = (PacketHeader *)buff;

	// 클라이언트용이 아님?? 그럼 끝
	if (header->signature != CLIENT_SIG)
		return FALSE;

	// 패킷 시리얼이 다르면 끝
// 	if( header->packetserial > packetSerial)
// 		return FALSE;

	if (header->packetsize > PACKET_MAX_SIZE)
		return FALSE;

	return TRUE;
}

// 해당 세션에게 보내기위하여 임시저장 (lockfree)
BOOL Session::sendBuffer(DataBuff *buff)
{
	sendBuffList.push(buff);
	return TRUE;
}


BOOL Session::isConnected()
{
	return connected && !uv_is_closing((uv_handle_t*)&socket);
}




