
#include "Session.h"
#include "Logger.h"
#include "IHandler.h"
#include "Packet.h"

// libuv���� packet�� Read�� buffer�� �˷��ش�. �̰��� call�� �Ŀ� �ڵ����� libuv������ packet�� buf�� ä���.
void On_AllocBuff(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	Session* session = (Session*)handle->data;
	buf->base = session->recvBuf + session->dataLen;
	buf->len = RECV_BUF_SIZE - session->dataLen;
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
		session->handleRecvPacket(readsize);
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
	{
		//����
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
		// ����
	}
	else
	{
		//����
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
		// ������ ���� ���� �־ �̾ �޴´�
		if (postPacketsize > 0)
		{
			if (postDataLen + dataLen < postPacketsize)
			{
				// �̾ ������ ���� �����ϴ�.
				memcpy(postRecvBuf + postDataLen, buffReadPos, dataLen);
				postDataLen += dataLen;
				buffReadPos = recvBuf;
				dataLen = 0;
				break;
			}
			else
			{
				// �̾ �޴ٺ��� packet ũ�⸦ �������״�.
				INT32 _readsize = postPacketsize - postDataLen;
				memcpy(postRecvBuf + postDataLen, buffReadPos, _readsize);

				dataLen -= _readsize;
				buffReadPos += _readsize;
				postDataLen += _readsize;

				postPacketsize = 0;
				// ��Ŷ ó�� �ڵ鷯���� ����
				dataHandler->onDataHandle(m_pCurRecvBuffer, this);
			}
		}

		//////////////////////////////////////////////////////////////////////////

		// ��� ���� �������� ������ ���޾ƾ� �ϱ⶧���� ��
		if(dataLen < sizeof(PacketHeader))
			break;

		PacketHeader* header = (PacketHeader*)buffReadPos;

		// ����� ��ȿ���� ���� ��� FALSE
		if (checkPacketHeader(buffReadPos) == FALSE)
			return FALSE;
		
		UINT32 packetSize = header->packetsize;

		// ���� ���޾Ҵ� (��Ŷ�� ���̺��� ���� ���۰� �۴�)
		if ((packetSize> dataLen) && (packetSize < RECV_BUF_SIZE))
			break;

		if (packetSize <= dataLen)
		{
			// ��Ŷ�� ����� ����
			memcpy(postRecvBuf, buffReadPos, packetSize);
			postDataLen = packetSize;

			dataLen -= packetSize;
			buffReadPos += packetSize;

			// TODO. packet ó��
			//m_pDataHandler->OnDataHandle(pDataBuffer, this);
		}
		else
		{
			// ���� ��Ŷ�� �� �޾Ҵ�, ���� ��ŭ�� �ϴ� �����ϰ� ������ ������ �� loop ������ ó��
			memcpy(postRecvBuf, buffReadPos, dataLen);
			postDataLen = dataLen;

			dataLen = 0;
			buffReadPos = recvBuf;
			postPacketsize = packetSize;
		}
	}

	return TRUE;
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
