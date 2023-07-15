#pragma once

#ifndef __RPC_CHANNEL_H__
#define __RPC_CHANNEL_H__

#include <google/protobuf/service.h>

class RpcChannel : public google::protobuf::RpcChannel
{
public:
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done);
};

#endif