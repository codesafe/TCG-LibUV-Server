
#include "Session.h"
#include "Logger.h"
#include "IHandler.h"
#include "Packet.h"

// libuv에게 packet을 Read할 buffer를 알려준다. 이곳이 call된 후에 자동으로 libuv에서는 packet을 buf에 채운다.
void On_AllocBuff(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	Session* session = (Session*)handle->data;
	buf->base = session->recvBuf + session->dataLen;
	buf->len = RECV_BUF_SIZE - session->dataLen;
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
		session->handleRecvPacket(readsize);
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
	{
		//실패
	}

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
	{
		//실패
	}

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
	dataHandler = NULL;
	dataLen = 0;
	memset(recvBuf, 0, RECV_BUF_SIZE);
	buffReadPos = recvBuf;

	postDataLen = 0;
	postPacketsize = 0;
	memset(postRecvBuf, 0, RECV_BUF_SIZE);

	packetSerial = 0;

	//m_bConnected = FALSE;
	//m_u64ConnData = 0;
	//connID = 0;

	//curRecvBuffer = NULL;

	//m_nCheckNo = 0;
	//m_IsSending = FALSE;
	//sendingBuffer = NULL;

	socket.data = (void*)this;
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
	
	dataLen = 0;

	//m_IsSending = FALSE;
	if (dataHandler != NULL)
		dataHandler->onCloseConnect(this);

	//m_bConnected = FALSE;

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
		//ASSERT_FAIELD;
		return FALSE;
	}
	return TRUE;
}

BOOL Session::doSend()
{
/*
	m_IsSending = TRUE;

	if (m_pSendingBuffer != NULL)
	{
		m_pSendingBuffer->Release();
		m_pSendingBuffer = NULL;
	}

	IDataBuffer* pFirstBuff = NULL;
	int nSendSize = 0;
	int nCurPos = 0;

	IDataBuffer* pBuffer = NULL;
	while (m_SendBuffList.pop(pBuffer))
	{
		nSendSize += pBuffer->GetTotalLenth();

		if (pFirstBuff == NULL && m_pSendingBuffer == NULL)
		{
			pFirstBuff = pBuffer;

			if (nSendSize >= RECV_BUF_SIZE)
			{
				m_pSendingBuffer = pBuffer;
				break;
			}

			pBuffer = NULL;
		}
		else
		{
			if (m_pSendingBuffer == NULL)
			{
				m_pSendingBuffer = CBufferAllocator::GetInstancePtr()->AllocDataBuff(RECV_BUF_SIZE);
				pFirstBuff->CopyTo(m_pSendingBuffer->GetBuffer() + nCurPos, pFirstBuff->GetTotalLenth());
				m_pSendingBuffer->SetTotalLenth(m_pSendingBuffer->GetTotalLenth() + pFirstBuff->GetTotalLenth());
				nCurPos += pFirstBuff->GetTotalLenth();
				pFirstBuff->Release();
				pFirstBuff = NULL;
			}

			pBuffer->CopyTo(m_pSendingBuffer->GetBuffer() + nCurPos, pBuffer->GetTotalLenth());
			m_pSendingBuffer->SetTotalLenth(m_pSendingBuffer->GetTotalLenth() + pBuffer->GetTotalLenth());
			nCurPos += pBuffer->GetTotalLenth();
			pBuffer->Release();
			pBuffer = NULL;
			if (nSendSize >= RECV_BUF_SIZE)
			{
				break;
			}
		}
	}

	if (m_pSendingBuffer == NULL)
	{
		m_pSendingBuffer = pFirstBuff;
	}

	if (m_pSendingBuffer == NULL)
	{
		m_IsSending = FALSE;
		return FALSE;
	}

	m_WriteReq.data = (void*)this;
	uv_buf_t buf = uv_buf_init(m_pSendingBuffer->GetBuffer(), m_pSendingBuffer->GetBufferSize());
	uv_write(&m_WriteReq, (uv_stream_t*)&socket, &buf, 1, On_WriteData);

*/

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


BOOL Session::setDataHandler(IPacketHandler* handler)
{
	//ERROR_RETURN_FALSE(pHandler != NULL);

	dataHandler = handler;

	return TRUE;
}

BOOL Session::handleRecvPacket(INT32 readsize)
{
	while (1)
	{
		// 이전에 받은 것이 있어서 이어서 받는다
		if (postPacketsize > 0)
		{
			if (postDataLen + dataLen < postPacketsize)
			{
				// 이어서 받지만 아직 부족하다.
				memcpy(postRecvBuf + postDataLen, buffReadPos, dataLen);
				postDataLen += dataLen;
				buffReadPos = recvBuf;
				dataLen = 0;
				break;
			}
			else
			{
				// 이어서 받다보니 packet 크기를 충족시켰다.
				INT32 _readsize = postPacketsize - postDataLen;
				memcpy(postRecvBuf + postDataLen, buffReadPos, _readsize);

				dataLen -= _readsize;
				buffReadPos += _readsize;
				postDataLen += _readsize;

				postPacketsize = 0;
				// 패킷 처리 핸들러에게 전달
				dataHandler->onDataHandle(m_pCurRecvBuffer, this);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// 헤더 보다 받은것이 작으면 더받아야 하기때문에 끝
		if(dataLen < sizeof(PacketHeader))
			break;

		PacketHeader* header = (PacketHeader*)buffReadPos;

		// 헤더가 유효하지 않은 경우 FALSE
		if (checkPacketHeader(buffReadPos) == FALSE)
			return FALSE;
		
		UINT32 packetSize = header->packetsize;

		// 아직 덜받았다 (패킷의 길이보다 받은 버퍼가 작다)
		if ((packetSize> dataLen) && (packetSize < RECV_BUF_SIZE))
			break;

		if (packetSize <= dataLen)
		{
			// 패킷을 충분히 받음
			memcpy(postRecvBuf, buffReadPos, packetSize);
			postDataLen = packetSize;

			dataLen -= packetSize;
			buffReadPos += packetSize;

			// TODO. packet 처리
			//m_pDataHandler->OnDataHandle(pDataBuffer, this);
		}
		else
		{
			// 아직 패킷을 덜 받았다, 받은 만큼을 일단 보관하고 나머지 수신인 이 loop 위에서 처리
			memcpy(postRecvBuf, buffReadPos, dataLen);
			postDataLen = dataLen;

			dataLen = 0;
			buffReadPos = recvBuf;
			postPacketsize = packetSize;
		}
	}

	return TRUE;
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
