#ifndef __RPC_APPLICATION_H__
#define __RPC_APPLICATION_H__

#include "rpc_controller.h"
#include "noncopyable.h"
#include "singleton.h"

class RpcApplication : public sylar::Noncopyable
{
public:
    void Init();

    void setRpcIp(const std::string &val) { rpcip = val; }
    const std::string &getRpcIp() const { return rpcip; }
    void setRpcPort(int val) { rpcport = val; }
    int getRpcPort() const { return rpcport; }
    void setZkIp(const std::string &val) { zkip = val; }
    const std::string &getZkIp() const { return zkip; }
    void setZkPort(int val) { zkport = val; }
    int getZkPort() const { return zkport; }

private:
    std::string rpcip;
    int rpcport;

    std::string zkip;
    int zkport;
};

typedef sylar::Singleton<RpcApplication> RpcApplicationMgr;

#endif