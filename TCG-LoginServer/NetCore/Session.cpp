
#include "Session.h"
#include "IHandler.h"
#include "DataBufferPool.h"
#include "../Common/Logger.h"
#include "../Packet.h"



// libuv���� packet�� Read�� buffer�� �˷��ش�. �̰��� call�� �Ŀ� �ڵ����� libuv������ packet�� buf�� ä���.
void On_AllocBuff(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	Session* session = (Session*)handle->data;
	buf->base = session->recvBuf + session->recvDataLen;
	buf->len = RECV_BUF_SIZE - session->recvDataLen;
	return;
}

// Packet�� �ް� ���� ��� (�̰��� call�Ǹ� �̹� buf�� packet�� �������̴�)
void On_ResultReadData(uv_stream_t* stream, ssize_t readsize, const uv_buf_t* buf)
{
	Session* session = (Session*)stream->data;
	assert(session);

	if (readsize >= 0)
	{
		Log::instance()->LogInfo("LibUV Recv Data %d : %s : %d byte", session->getSessionID(), buf->base, readsize);
		// Recv���� ó��
		session->handleRecvPacket((INT32)readsize);
	}
	else
	{
		// read�� -�� ������ ����
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
		//����
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
		// ����
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


// sendBuf�� �������� ��ä���� ������.
BOOL Session::doSend()
{
	isSending = TRUE;

	DataBuff *currentbuff = nullptr;
	UINT32 bufferlen = 0;

	sendDataLen = 0;

	// ������ �����ٰ� �������� ������ �װ��� �������� sendBuf�� ����
	if (prevSendBuf != nullptr)
	{
		UINT32 _len = prevSendBuf->buffersize - prevSendOffset;
		memcpy(sendBuf, prevSendBuf + prevSendOffset, _len);
		sendDataLen = _len;

		prevSendBuf->release();
		prevSendBuf = nullptr;
		prevSendOffset = 0;
	}

	// ������ �Ұ��� ������ pop�Ͽ� ������
	while (sendBuffList.pop(currentbuff))
	{
		bufferlen = currentbuff->buffersize;

		if (sendDataLen + bufferlen >= SEND_BUF_SIZE)
		{
			// �����ٺ��� ���۰� ��ģ��. �������� ������ ������.
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

			// ��á���� ������
			if( sendDataLen >= SEND_BUF_SIZE )
				break;
		}
	}

	// ���̻� �������� ������ return FALSE
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
		// ������ ���� ���� �־ �̾ �޴´�
		if (postPacketsize > 0)
		{
			if (postDataLen + recvDataLen < postPacketsize)
			{
				// �̾ ������ ���� �����ϴ�.
				memcpy(postRecvBuf + postDataLen, buffReadPos, recvDataLen);
				postDataLen += recvDataLen;
				buffReadPos = recvBuf;
				recvDataLen = 0;
				break;
			}
			else
			{
				// �̾ �޴ٺ��� packet ũ�⸦ �������״�.
				INT32 _readsize = postPacketsize - postDataLen;
				memcpy(postRecvBuf + postDataLen, buffReadPos, _readsize);

				recvDataLen -= _readsize;
				buffReadPos += _readsize;
				postDataLen += _readsize;

				// ��Ŷ ó�� �ڵ鷯���� ����
				recvDataToHander();
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// ��� ���� �������� ������ ���޾ƾ� �ϱ⶧���� ��
		if(recvDataLen < sizeof(PacketHeader))
			break;

		PacketHeader* header = (PacketHeader*)buffReadPos;

		// ����� ��ȿ���� ���� ��� FALSE
		if (checkPacketHeader(buffReadPos) == FALSE)
			return FALSE;
		
		UINT32 packetSize = header->packetsize;

		// ���� ���޾Ҵ� (��Ŷ�� ���̺��� ���� ���۰� �۴�)
		if ((packetSize> recvDataLen) && (packetSize < RECV_BUF_SIZE))
			break;

		if (packetSize <= recvDataLen)
		{
			// ��Ŷ�� ����� ����
			memcpy(postRecvBuf, buffReadPos, packetSize);
			postDataLen = packetSize;

			recvDataLen -= packetSize;
			buffReadPos += packetSize;

			// ��Ŷ ó�� �ڵ鷯���� ����
			recvDataToHander();
		}
		else
		{
			// ���� ��Ŷ�� �� �޾Ҵ�, ���� ��ŭ�� �ϴ� �����ϰ� ������ ������ �� loop ������ ó��
			memcpy(postRecvBuf, buffReadPos, recvDataLen);
			postDataLen = recvDataLen;

			recvDataLen = 0;
			buffReadPos = recvBuf;
			postPacketsize = packetSize;
		}
	}

	return TRUE;
}

// ��Ŷ ó�� �ڵ鷯���� ����
VOID Session::recvDataToHander()
{
	DataBuff * databuff = DataBufferManager::instance()->allocBuffer(100);
	databuff->copyData(postRecvBuf, 0, postDataLen);
	handler->onRecvData(databuff, this);
	postPacketsize = 0;
	postDataLen = 0;
}

// ��Ŷ�� ����� ����
BOOL Session::checkPacketHeader(CHAR *buff)
{
/*
	�˻� �׸�
	Packet �����ڵ� (������/Ŭ���̾�Ʈ��)
	Packet ����
	Packet �Ϸù�ȣ (���������� �ʰų�, �ߺ�����)
	Packet ��ȣȭ ����

	�˻� �׸񿡼� �ϳ��� �߸��Ǹ� �ش� ������ ��ȿȭ(Close) �Ѵ�.
	��Ŷ�� �����Ͽ� Server�� �����ϰų� �Ҽ� �ֱ� ������ �� �κ��� �߿�
	* ��Ŷ�� ���̰� �ʹ� ��ų�, ���̰� �ȸ´°��, ���̴� ������ ������ ������ ������ �����͵� ����
*/
	PacketHeader *header = (PacketHeader *)buff;

	// Ŭ���̾�Ʈ���� �ƴ�?? �׷� ��
	if (header->signature != CLIENT_SIG)
		return FALSE;

	// ��Ŷ �ø����� �ٸ��� ��
// 	if( header->packetserial > packetSerial)
// 		return FALSE;

	if (header->packetsize > PACKET_MAX_SIZE)
		return FALSE;

	return TRUE;
}

// �ش� ���ǿ��� ���������Ͽ� �ӽ����� (lockfree)
BOOL Session::sendBuffer(DataBuff *buff)
{
	sendBuffList.push(buff);
	return TRUE;
}


BOOL Session::isConnected()
{
	return connected && !uv_is_closing((uv_handle_t*)&socket);
}




