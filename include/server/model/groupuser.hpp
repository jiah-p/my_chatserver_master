#ifndef GROUPUSER_H
#define GROUPUSER_H

#include "user.hpp"
/*
    GroupUser 表 
    groupid int not null union Pramary Key
    userid int not null union Pramary Key
    grouprole ENUM('creator', 'normal') default 'normal'
*/
// 群组用户，多了一个role角色信息，从User类直接继承，复用User的其它信息
// 显示用户登陆的情况下的组的信息
class GroupUser : public User
{
public:
    void setrole(string role) { this->role = role; }
    string getrole() { return this->role; }

private:
// 继承User 多一个role角色
    string role;
};

#endif