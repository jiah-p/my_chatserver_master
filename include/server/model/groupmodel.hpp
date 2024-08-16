#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

// 维护群组信息的操作接口方法
class GroupModel
{
public:
    // 创建群组 
    bool creategroup(Group &group);

    // 加入群组
    void addgroup(int userid, int groupid, string role);

    // 查询用户所在群组信息
    vector<Group> querygroups(int userid);

    // 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
    // 获取群内其他人员的id
    vector<int> querygroupUsers(int userid, int groupid);
};

#endif