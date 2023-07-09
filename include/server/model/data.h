#ifndef __DATA_H__
#define __DATA_H__

#include <string>
#include <vector>

class User
{
public:
    User(int id = -1, std::string name = "",
         std::string password = "", std::string state = "offline")
    {
        m_id = id;
        m_name = name;
        m_password = password;
        m_state = state;
    }

    void setId(int id) { m_id = id; }
    int getId() { return m_id; }

    void setName(std::string name) { m_name = name; }
    const std::string &getName() const { return m_name; }

    void setPassword(std::string password) { m_password = password; }
    const std::string &getPassword() const { return m_password; }

    void setState(std::string state) { m_state = state; }
    const std::string &getState() const { return m_state; }

private:
    int m_id;
    std::string m_name;
    std::string m_password;
    std::string m_state;
};

class GroupUser : public User
{
public:
    void setRole(std::string role) { m_role = role; }
    const std::string &getRole() const { return m_role; }

private:
    std::string m_role;
};

class Group
{
public:
    Group(int id = -1, std::string name = "", std::string desc = "")
        : m_id(id), m_name(name), m_desc(desc) {}

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

    void setName(std::string name) { m_name = name; }
    const std::string &getName() const { return m_name; }

    void setDesc(std::string desc) { m_desc = desc; }
    const std::string &getDesc() const { return m_desc; }

    std::vector<GroupUser> &getUsers() { return m_users; }

private:
    int m_id;
    std::string m_name;
    std::string m_desc;
    std::vector<GroupUser> m_users;
};

#endif