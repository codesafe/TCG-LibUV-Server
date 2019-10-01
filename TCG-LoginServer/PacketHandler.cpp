
#include "Predef.h"
#include "PacketHandler.h"
#include "CommandDef.h"
#include "ServerCore.h"


PacketHandler::PacketHandler()
{

}

PacketHandler::~PacketHandler()
{

}

// client�� ���� ���ŵ� packet�� ���� ó��
// ex) ���� �α��� ��Ŷ�ǰ�� account ���� ������ ���� �ΰų� �ϴ°�찡 ����Ƿ� �̰����� ó��
// �α��� �������� ���ο��� ó���Ѵٸ� �̰����� ���ʿ䰡 ������??
BOOL PacketHandler::dispatchPacket(NetPacket* packet)
{
	switch (packet->msgID)
	{
		PROCESS_MESSAGE_ITEM(MSG_REQ_CREATE_ACCOUNT, onCreateAccountReq);
		PROCESS_MESSAGE_ITEM(MSG_REQ_LOGIN, onLoginReq);
	}
	return FALSE;
}

BOOL PacketHandler::onCreateAccountReq(NetPacket* packet)
{
	// TODO. check user & create account
	/*
	
	
	*/
	AckCreateAccount ack;
	ack.set_result(RET_CREATE_SUCCESS);
	return ServerCore::instance()->sendMsgProtoBuf(packet->sessionID, MSG_ACK_CREATE_ACCOUNT, 0, 0, ack);
}

BOOL PacketHandler::onLoginReq(NetPacket* packet)
{
	// TODO. check login user
	/*
		������ �α��� ó��.
		* DB�� �ִ� ���������ΰ�?
		* �̹� �α��� �Ǿ��ֳ�? --> �������� ���ų� / �̹� �α��� �Ǿ������� ������ �α��� ����
		* Ȯ�ε� ������ Cookie ���� --> redis�� ���� �ϰ� �α��� ����
		* �α����� �߰����� ���� �߰��Ͽ� ����
		* ��
	*/
	AckLogin ack;
	ack.set_result(RET_LOGIN_SUCCESS);
	return ServerCore::instance()->sendMsgProtoBuf(packet->sessionID, MSG_ACK_LOGIN, 0, 0, ack);
}
