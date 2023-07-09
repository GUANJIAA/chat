#ifndef __DATAMODEL_H__
#define __DATAMODEL_H__

#include "data.h"

#include <string>
#include <vector>

class UserModel
{
public:
    bool insert(User &user);
    User query(int id);
    bool updateState(User user);
    void resetState();
};

class offlineMsgModel
{
public:
    void insert(int userid, std::string msg);
    void remove(int userid);
    std::vector<std::string> query(int userid);
};

class GroupModel
{
public:
    bool createGroup(Group &group);
    void addGroup(int userid, int groupid, std::string role);
    std::vector<Group> queryGroups(int userid);
    std::vector<int> queryGroups(int userid, int groupid);
};

class FriendModel
{
public:
    void insert(int userid, int friendid);

    std::vector<User> query(int userid);
};

#endif