#ifndef GROUP_H
#define GROUP_H

#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;


/*
    Group 表
    id int PRIMARY KEY, AUTO_INCR
    groupname  varchar(50)  not null unique
    groupdesc varchar(200)  default ''
*/
// User表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }

    void setid(int id) { this->id = id; }
    void setname(string name) { this->name = name; }
    void setdesc(string desc) { this->desc = desc; }

    int getid() { return this->id; }
    string getname() { return this->name; }
    string getdesc() { return this->desc; }
    vector<GroupUser> &getusers() { return this->users; }

private:

    int id;
    string name;
    string desc;
    vector<GroupUser> users;   // 存放了 组内人员的信息 
    
/*
GroupUser : Users
    string role
    
    string _name;
    uint64_t _id;
    string _password;
    string _state;
*/
 
    /*
        Group 表
        id int PRIMARY KEY, AUTO_INCR
        groupname  varchar(50)  not null unique
        groupdesc varchar(200)  default ''
    */

};

#endif