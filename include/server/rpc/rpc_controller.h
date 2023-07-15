#pragma once

#ifndef __RPC_CONTROLLER_H__
#define __RPC_CONTROLLER_H__

#include <google/protobuf/service.h>
#include <string>

class RpcController : public google::protobuf::RpcController
{
public:
    RpcController();
    void Reset();
    bool Failed() const { return m_failed; }
    std::string ErrorText() const { return m_errText; }
    void SetFailed(const std::string &reason);

private:
    bool m_failed;
    std::string m_errText;
};

#endif