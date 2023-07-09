#ifndef __MYSQLDB_H__
#define __MYSQLDB_H__

#include <mysql/mysql.h>
#include <string>
#include <memory>
#include <list>
#include <semaphore.h>
#include <pthread.h>

#include "singleton.h"

class MySQL
{
public:
    typedef std::shared_ptr<MySQL> ptr;

    MySQL();
    ~MySQL();

    bool connect();
    bool update(std::string sql);
    MYSQL_RES *query(std::string sql);
    MYSQL *getConnection();

private:
    MYSQL *m_conn;
};

class connection_pool
{
public:
    connection_pool()
    {
        m_CurConn = 0;
        m_FreeConn = 0;
    }

    MySQL *GetConnection();
    bool ReleaseConnection(MySQL *conn);
    void DestroyPool();

    void init();

private:
    int m_MaxConn;
    int m_CurConn;
    int m_FreeConn;
    std::list<MySQL *> connList;
    sem_t m_sem;
    pthread_mutex_t m_mutex;
};

typedef sylar::Singleton<connection_pool> MySQLConnPoolMgr;

#endif