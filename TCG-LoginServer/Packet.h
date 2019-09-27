#ifndef PACKET
#define PACKET

#include "Predef.h"

#pragma  pack(push)
#pragma  pack(1)

#define CLIENT_SIG 0xc0
#define SERVER_SIG 0xde

struct PacketHeader
{
	UINT8	signature;		// 서버가 보낸것?(SERVER_SIG) 클라이언트가 보낸것? (CLIENT_SIG)
	UINT32	packetsize;		// packet size는 header + data 포함한 전체 길이
	UINT32	packetserial;	//생성 번호 = wCommandID^dwSize+index(패키지당 자동 성장 색인); 환원 번호 = pHeader->dwPacketNo - pHeader->wCommandID^pHeader->dwSize;
	UINT32	msgID;			// msg ID

//	UINT32   dwMsgID;
//	UINT64   u64TargetID;
//	UINT32   dwUserData;	//클라이언트가 사본에 있는 경우，dwUserData是copyguid， 논리복의 상황에서， 일부 프로토콜은 클라이언트 연결을 충당합니다ID
};

#pragma  pack(pop)

#endif
