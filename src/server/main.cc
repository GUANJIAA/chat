#include "mysqldb.h"
#include "config.h"
#include <iostream>

int main(int argc, char **argv)
{
    sylar::Config::LoadFromConfDir("/root/projects/examples/chat_room/bin");
    MySQLConnPoolMgr::GetInstance()->init();
    for (int i = 0; i < 2; ++i)
    {
        MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
        MYSQL_RES *res = mysql->query("select * from user");
        MYSQL_ROW row = mysql_fetch_row(res);
        std::cout << row[0] << std::endl;
        std::cout << row[1] << std::endl;
        std::cout << row[2] << std::endl;
        std::cout << row[3] << std::endl;
        std::cout << row[4] << std::endl;
        mysql_free_result(res);
    }

    return 0;
}