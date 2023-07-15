#include "rpc_channel.h"
#include "zk_util.h"
#include "rpc_header.pb.h"
#include "log.h"
#include "socket.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>

static sylar::Logger::ptr rpc_logger = SYLAR_LOG_NAME("rpc");

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                            google::protobuf::RpcController *controller,
                            const google::protobuf::Message *request,
                            google::protobuf::Message *response,
                            google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    int args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error");
        return;
    }

    RPC::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serializze rpc header error");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    SYLAR_LOG_INFO(rpc_logger) << "header_size: " << header_size
                               << " rpc_header_str: " << rpc_header_str
                               << " service_name: " << service_name
                               << " method_name: " << method_name
                               << " args_str: " << args_str.c_str();

    sylar::Socket::ptr client = sylar::Socket::CreateTCPSocket();

    ZkClient zkCli;
    zkCli.Start();
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    sylar::IPAddress::ptr addr = sylar::Address::LookupIPAdress(ip + ":" + std::to_string(port));
    if (!client->connect(addr))
    {
        client->close();
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if (!client->send(send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        client->close();
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    char recv_buf[1024] = {0};
    int recv_size = client->recv(recv_buf, 1024, 0);
    if (-1 == recv_size)
    {
        client->close();
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        client->close();
        char errtxt[512] = {0};
        sprintf(errtxt, "ParseFromArray error! response)str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    client->close();
}