#include "rpc_controller.h"

RpcController::RpcController()
    : m_failed(false), m_errText(std::string("")) {}

void RpcController::Reset()
{
    m_failed = false;
    m_errText = "";
}

void RpcController::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}