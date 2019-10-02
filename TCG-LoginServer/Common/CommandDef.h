#ifndef MSG_DEFINE
#define MSG_DEFINE

#include "Logger.h"
#include "../NetCore/IHandler.h"

#define BEGIN_PROCESS_MESSAGE(ClassName) \
BOOL ClassName##::dispatchPacket(NetPacket *packet) \
{ \
	PacketHeader *pPacketHeader = (PacketHeader *)packet->dataBuffer->GetBuffer();\
	ERROR_RETURN_TRUE(pPacketHeader != NULL);\
	switch(pPacketHeader->msgID) \
	{

#define PROCESS_MESSAGE_ITEM(msgID, Func) \
		case msgID:{\
		Log::instance()->LogInfo("---Receive Message:[%s]----", #msgID);\
		if(Func(packet)){return TRUE;}}break;

#define PROCESS_MESSAGE_ITEMEX(msgID, Func) \
		case msgID:{\
		if(Func(packet)){return TRUE;}}break;

#define END_PROCESS_MESSAGE \
		default: \
			{ return FALSE;} \
			break;\
	}\
	return TRUE;\
}

#endif



