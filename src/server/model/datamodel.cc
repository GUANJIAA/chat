#include "mysqldb.h"
#include "datamodel.h"
#include "log.h"

bool UserModel::insert(User &user)
{
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')",
            user.getName().c_str(), user.getPassword().c_str(), user.getState().c_str());

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    bool result = false;
    if (mysql->connect())
    {
        if (mysql->update(sql))
        {
            user.setId(mysql_insert_id(mysql->getConnection()));
            result = true;
        }
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    if (mysql->connect())
    {
        MYSQL_RES *res = mysql->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);

                if (res != nullptr)
                {
                    mysql_free_result(res);
                    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
                    return user;
                }
            }
        }
    }
    return NULL;
}