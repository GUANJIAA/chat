#ifndef __LOGINSERVER_H__
#define __LOGINSERVER_H__

#include "json.hpp"
#include "sylar/tcp_server.h"
#include "sylar/singleton.h"

#include <functional>
#include <mutex>

class LoginServer
{
public:
    
};

typedef sylar::Singleton<LoginServer> LoginServerMgr;

#endif