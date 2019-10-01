
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

// client로 부터 수신된 packet의 실제 처리
// ex) 만일 로그인 패킷의경우 account 검증 서버를 따로 두거나 하는경우가 생기므로 이곳에서 처리
// 로그인 같은것을 내부에서 처리한다면 이곳까지 올필요가 있을까??
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
		유저의 로그인 처리.
		* DB에 있는 정상유저인가?
		* 이미 로그인 되어있나? --> 기존유저 끊거나 / 이미 로그인 되어있으니 현재의 로그인 실패
		* 확인된 유저면 Cookie 생성 --> redis에 저장 하고 로그인 성공
		* 로그인후 추가적인 정보 추가하여 전송
		* 끝
	*/
	AckLogin ack;
	ack.set_result(RET_LOGIN_SUCCESS);
	return ServerCore::instance()->sendMsgProtoBuf(packet->sessionID, MSG_ACK_LOGIN, 0, 0, ack);
}
