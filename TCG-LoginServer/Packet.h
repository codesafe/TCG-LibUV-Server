#ifndef PACKET
#define PACKET

#include "Predef.h"

#pragma  pack(push)
#pragma  pack(1)

#define CLIENT_SIG 0xc0
#define SERVER_SIG 0xde

struct PacketHeader
{
	UINT8	signature;		// ������ ������?(SERVER_SIG) Ŭ���̾�Ʈ�� ������? (CLIENT_SIG)
	UINT32	packetsize;		// packet size�� header + data ������ ��ü ����
	UINT32	packetserial;	//���� ��ȣ = wCommandID^dwSize+index(��Ű���� �ڵ� ���� ����); ȯ�� ��ȣ = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	UINT32	msgID;			// msg ID

//	UINT32   dwMsgID;
//	UINT64   u64TargetID;
//	UINT32   dwUserData;	//Ŭ���̾�Ʈ�� �纻�� �ִ� ��죬dwUserData��copyguid�� ������ ��Ȳ������ �Ϻ� ���������� Ŭ���̾�Ʈ ������ ����մϴ�ID
};

#pragma  pack(pop)

#endif
