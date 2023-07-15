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
    if (mysql->update(sql))
    {
        user.setId(mysql_insert_id(mysql->getConnection()));
        result = true;
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return false;
}

User UserModel::query(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = %d", id);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
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
    return NULL;
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    bool result = false;
    if (mysql->update(sql))
    {
        result = true;
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return result;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user set state = 'offline' where state = 'online'");

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    mysql->update(sql);
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
}

// 存储用户的离线消息
void OfflineMsgModel::insert(int userid, std::string msg)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    mysql->update(sql);
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
}

// 删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    mysql->update(sql);
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
}

// 查询用户的离线消息
std::vector<std::string> OfflineMsgModel::query(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    std::vector<std::string> vec;
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        // 把userid用户的所有离线消息放入vec中返回
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(row[0]);
        }

        mysql_free_result(res);
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return vec;
}

// 创建群组
bool GroupModel::createGroup(Group &group)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    // insert into allgroup(groupname,groupdesc) value('%s','%s')
    sprintf(sql, "insert into allgroup(groupname,groupdesc) value('%s','%s')",
            group.getName().c_str(), group.getDesc().c_str());

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    bool result = false;
    if (mysql->update(sql))
    {
        group.setId(mysql_insert_id(mysql->getConnection()));
        result = true;
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return result;
}

// 加入群组
void GroupModel::addGroup(int userid, int groupid, std::string role)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    // insert into groupuser values(%d,%d,'%s')
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')",
            groupid, userid, role.c_str());

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    mysql->update(sql);
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
}

// 查询用户所在群组信息
std::vector<Group> GroupModel::queryGroups(int userid)
{
    /*
        1.先根据userid在groupuser表中查询出该用户所属的群组信息
        2.再根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    char sql[1024] = {0};
    // select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id = b.groupid where b.userid = %d",
            userid);

    std::vector<Group> groupVec;

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        // 查出userid所有的群组信息
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setId(atoi(row[0]));
            group.setName(row[1]);
            group.setDesc(row[2]);
            groupVec.push_back(group);
        }
        mysql_free_result(res);
    }

    // 查询群组的用户信息
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a \
        inner join groupuser b on b.userid=a.id where b.groupid=%d",
                group.getId());

        MYSQL_RES *res = mysql->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其他成员群发消息
std::vector<int> GroupModel::queryGroupsUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    std::vector<int> idVec;
    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            idVec.push_back(atoi(row[0]));
        }
        mysql_free_result(res);
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return idVec;
}

// 添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d,%d)", userid, friendid);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    mysql->update(sql);
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
}

// 返回用户好友列表 friendid
std::vector<User> FriendModel::query(int userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    // select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid = %d", userid);

    MySQL *mysql = MySQLConnPoolMgr::GetInstance()->GetConnection();
    std::vector<User> vec;
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        // 把userid用户的所有离线消息放入vec中返回
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
    }
    MySQLConnPoolMgr::GetInstance()->ReleaseConnection(mysql);
    return vec;
}