#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include <vector>

using namespace std;

// 维护好友信息的操作与接口类
class FriendModel{
public:
    // 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户好友列表 注意真实场景 应该通过加密存在本地环境中
    // 需要进行内联查询
    vector<User> query(int userid);
};

#endif