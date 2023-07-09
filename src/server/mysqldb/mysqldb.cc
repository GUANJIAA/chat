#include "mysqldb.h"
#include "log.h"
#include "config.h"

static sylar::Logger::ptr c_logger = SYLAR_LOG_NAME("CHAT");

struct MYSQLInfo
{
    std::string server;
    std::string user;
    std::string password;
    std::string dbname;
    uint port = 3306;
    uint dbPoolSize = 0;

    bool operator==(const MYSQLInfo &oth) const
    {
        return server == oth.server &&
               user == oth.user &&
               password == oth.password &&
               dbname == oth.dbname &&
               dbPoolSize == oth.dbPoolSize;
    }
};

namespace sylar
{
    template <>
    class LexicalCast<std::string, MYSQLInfo>
    {
    public:
        MYSQLInfo operator()(const std::string &val)
        {
            SYLAR_LOG_INFO(c_logger) << "test1";
            YAML::Node node = YAML::Load(val);
            MYSQLInfo conf;
            conf.server = node["server"].as<std::string>();
            conf.user = node["user"].as<std::string>();
            conf.password = node["password"].as<std::string>();
            conf.dbname = node["dbname"].as<std::string>();
            conf.dbPoolSize = (uint)node["dbpoolsize"].as<int>();
            conf.port = (uint)node["port"].as<int>();
            //  SYLAR_LOG_INFO(c_logger) << "conf.server: " << conf.server
            //                           << "conf.user: " << conf.user
            //                           << "conf.password: " << conf.password
            //                           << "conf.dbname: " << conf.dbname
            //                           << "conf.dbPoolSize: " << conf.dbPoolSize;
            return conf;
            SYLAR_LOG_INFO(c_logger) << "test1";
        }
    };

    template <>
    class LexicalCast<MYSQLInfo, std::string>
    {
    public:
        std::string operator()(const MYSQLInfo &val)
        {
            SYLAR_LOG_INFO(c_logger) << "test2";
            YAML::Node node;
            node["server"] = val.server;
            node["user"] = val.user;
            node["password"] = val.password;
            node["dbname"] = val.dbname;
            node["dbPoolSize"] = val.dbPoolSize;
            node["port"] = val.port;
            std::stringstream ss;
            ss << node;
            return ss.str();
        }
    };

}

static sylar::ConfigVar<MYSQLInfo>::ptr c_mysql_info =
    sylar::Config::Lookup("mysql", MYSQLInfo(), "mysql info");

// static MYSQLInfo c_mysql_info;

MySQL::MySQL()
{
    m_conn = mysql_init(nullptr);
    if (m_conn == nullptr)
    {
        SYLAR_LOG_ERROR(c_logger) << "mysql init fail";
    }
}

MySQL::~MySQL()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
}

bool MySQL::connect()
{
    const char *server = c_mysql_info->getValue().server.c_str();
    const char *user = c_mysql_info->getValue().user.c_str();
    const char *password = c_mysql_info->getValue().password.c_str();
    const char *dbname = c_mysql_info->getValue().dbname.c_str();

    // SYLAR_LOG_INFO(c_logger) << server;
    // SYLAR_LOG_INFO(c_logger) << user;
    // SYLAR_LOG_INFO(c_logger) << password;
    // SYLAR_LOG_INFO(c_logger) << dbname;

    MYSQL *p = mysql_real_connect(m_conn, server, user, password,
                                  dbname, 3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(m_conn, "set names gbk");
        SYLAR_LOG_INFO(c_logger) << "connect mysql success";
        return true;
    }
    else
    {
        SYLAR_LOG_ERROR(c_logger) << "connect mysql fail!";
        return false;
    }
}

bool MySQL::update(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        SYLAR_LOG_INFO(c_logger) << "update fail!";
        return false;
    }
    return true;
}

MYSQL_RES *MySQL::query(std::string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        SYLAR_LOG_INFO(c_logger) << "query fail!";
        return nullptr;
    }
    return mysql_use_result(m_conn);
}

MYSQL *MySQL::getConnection()
{
    return m_conn;
}

void connection_pool::init()
{
    int maxConn = c_mysql_info->getValue().dbPoolSize;
    for (int i = 0; i < maxConn; ++i)
    {
        MySQL *con(new MySQL);
        if (!con->connect())
        {
            continue;
        }
        connList.push_back(con);
        ++m_FreeConn;
    }
    sem_init(&m_sem, 0, m_FreeConn);
    pthread_mutex_init(&m_mutex, NULL);
}

// 当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MySQL *connection_pool::GetConnection()
{
    MySQL *con = NULL;
    if (connList.size() == 0)
    {
        return NULL;
    }
    sem_wait(&m_sem);

    pthread_mutex_lock(&m_mutex);
    con = connList.front();
    connList.pop_front();
    --m_FreeConn;
    ++m_CurConn;
    pthread_mutex_unlock(&m_mutex);

    return con;
}

bool connection_pool::ReleaseConnection(MySQL *con)
{
    if (con == NULL)
    {
        return false;
    }

    pthread_mutex_lock(&m_mutex);
    connList.push_back(con);
    ++m_FreeConn;
    --m_CurConn;
    pthread_mutex_unlock(&m_mutex);

    sem_post(&m_sem);
    return true;
}

void connection_pool::DestroyPool()
{
    pthread_mutex_lock(&m_mutex);
    if (connList.size() > 0)
    {
        for (auto it = connList.begin(); it != connList.end(); it++)
        {
            MySQL *con = *it;
            mysql_close(con->getConnection());
        }
        m_CurConn = 0;
        m_FreeConn = 0;
        connList.clear();
    }
    pthread_mutex_unlock(&m_mutex);
}