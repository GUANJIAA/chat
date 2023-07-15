#include "mysqldb.h"
#include "config.h"
#include "datamodel.h"
#include "log.h"
#include "rpc_application.h"
#include <iostream>

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int main(int argc, char **argv)
{
    sylar::Config::LoadFromConfDir("/root/projects/examples/chat_room/bin");
    RpcApplicationMgr::GetInstance()->Init();
    SYLAR_LOG_INFO(g_logger) << RpcApplicationMgr::GetInstance()->getRpcIp()
                             << RpcApplicationMgr::GetInstance()->getRpcPort()
                             << RpcApplicationMgr::GetInstance()->getZkIp()
                             << RpcApplicationMgr::GetInstance()->getZkPort();
    return 0;
}