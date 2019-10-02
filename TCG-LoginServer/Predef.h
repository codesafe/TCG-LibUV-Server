#pragma once

#include <stdio.h>

#include <map>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <assert.h>
#include <set>
#include <deque>
#include <mutex>
#include <thread>

#include "./Common/Platform.h"
#include "./Common/SpinLock.h"

#include "uv.h"

// Protobuf
#include "./pb/packet_def.pb.h"
#include "./pb/packet.pb.h"



#define MAX_BACKLOG		128		// listen backlog


// packet message id
#define MSG_NEW_CONNECTION		1
#define MSG_CLOSE_CONNECTION	2


#define PACKET_MAX_SIZE		1024 * 2	// 하나의 패킷 최대 길이(이걸 넘으면 안됨)
#define RECV_BUF_SIZE		4096 * 2
#define SEND_BUF_SIZE		RECV_BUF_SIZE


struct NetPacket;
typedef std::function<void(NetPacket* packet)> NET_RECEIVE_FUNCTOR;
typedef std::shared_ptr<NET_RECEIVE_FUNCTOR> NET_RECEIVE_FUNCTOR_PTR;