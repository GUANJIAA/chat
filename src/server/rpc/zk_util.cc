#include "zk_util.h"
#include "rpc_application.h"
#include "log.h"

#include <semaphore.h>

static sylar::Logger::ptr rpc_logger = SYLAR_LOG_NAME("rpc");

void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient()
    : m_zhandle(nullptr) {}

ZkClient::~ZkClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start()
{
    std::string host = RpcApplicationMgr::GetInstance()->getRpcIp();
    int port = RpcApplicationMgr::GetInstance()->getRpcPort();
    std::string connstr = host + ":" + std::to_string(port);

    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr)
    {
        SYLAR_LOG_ERROR(rpc_logger) << "zookeeper_init error";
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    SYLAR_LOG_INFO(rpc_logger) << "zookeeper_init success";
}

void ZkClient::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];
    int bufflen = sizeof(path_buffer);
    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (ZNONODE == flag)
    {
        flag = zoo_create(m_zhandle, path, data, datalen,
                          &ZOO_OPEN_ACL_UNSAFE, state, path_buffer, bufflen);
        if (flag == ZOK)
        {
            SYLAR_LOG_INFO(rpc_logger) << "znode create success... path:" << path;
        }
        else
        {
            SYLAR_LOG_ERROR(rpc_logger) << "flag: " << flag
                                        << "znode create error... path: " << path;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZkClient::GetData(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if (flag != ZOK)
    {
        SYLAR_LOG_ERROR(rpc_logger) << "get znode error... path: " << path;
        return "";
    }
    else
    {
        return buffer;
    }
}