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

#include "Platform.h"
#include "SpinLock.h"

#include "../Dep/libuv/uv.h"


#define MAX_BACKLOG		128		// listen backlog
