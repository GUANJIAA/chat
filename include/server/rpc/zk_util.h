#pragma once
#ifndef __ZK_UTIL_H__
#define __ZK_UTIL_H__

#include <string>
#include <zookeeper/zookeeper.h>
#include <semaphore.h>

class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void Start();
    void Create(const char *path, const char *data, int datalen, int state = 0);
    std::string GetData(const char *path);

private:
    zhandle_t *m_zhandle;
};

#endif