#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User表的数据的操作方法类
class UserModel{
public:
    //  User表的插入方法
    bool insert(User& user);

    //  User表的查询方法  返回普通对象调用拷贝构造，但是避免了局部变量的释放
    User query(int id);

    // 更新表的信息
    bool updatestate(User& user);

    // 重置用户状态信息
    void resetstate();
};

#endif