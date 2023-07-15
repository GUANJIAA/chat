#include "rpc_application.h"
#include "log.h"
#include "config.h"

static sylar::Logger::ptr rpc_logger = SYLAR_LOG_NAME("RPC");

struct RpcInfo
{
    std::string rpcIP;
    int rpcPort;
    std::string zkIP;
    int zkPort;

    bool operator==(const RpcInfo &oth) const
    {
        return rpcIP == oth.rpcIP &&
               rpcPort == oth.rpcPort &&
               zkIP == oth.zkIP &&
               zkPort == oth.zkPort;
    }
};

namespace sylar
{

    template <>
    class LexicalCast<std::string, RpcInfo>
    {
    public:
        RpcInfo operator()(const std::string &val)
        {
            YAML::Node node = YAML::Load(val);
            RpcInfo result;
            result.rpcIP = node["rpcip"].as<std::string>();
            result.rpcPort = node["rpcport"].as<int>();
            result.zkIP = node["zkip"].as<std::string>();
            result.zkPort = node["zkport"].as<int>();
            return result;
        }
    };

    template <>
    class LexicalCast<RpcInfo, std::string>
    {
    public:
        std::string operator()(const RpcInfo &val)
        {
            YAML::Node node;
            node["rpcip"] = val.rpcIP;
            node["rpcport"] = val.rpcPort;
            node["zkip"] = val.zkIP;
            node["zkport"] = val.zkPort;
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };
}

static sylar::ConfigVar<RpcInfo>::ptr rpc_deines =
    sylar::Config::Lookup("rpc", RpcInfo(), "rpc config");

void RpcApplication::Init()
{
    setRpcIp(rpc_deines->getValue().rpcIP);
    setRpcPort(rpc_deines->getValue().rpcPort);
    setZkIp(rpc_deines->getValue().zkIP);
    setZkPort(rpc_deines->getValue().zkPort);
}