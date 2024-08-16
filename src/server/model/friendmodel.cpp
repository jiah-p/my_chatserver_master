#include "friendmodel.hpp"
#include "db.h"

// 添加好友关系
void FriendModel::insert(int userid, int friendid){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)",
    userid, friendid);

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }

    return ;
}

// 返回用户好友列表 注意真实场景 应该通过加密存在本地环境中
// 需要进行内联查询
vector<User> FriendModel::query(int userid){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name, a.state from user a inner join friend b on  a.id == b.friendid where a.id == %d", userid);

    vector<User> vec;
    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr){
            // 查询成功
            // uint64_t STDCALL mysql_insert_id(MYSQL *mysql);
            MYSQL_ROW row;   // 得到查询的数据一行
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                // 根据表的结构和查询的语句分类得到数据 
                // 得到的是字符串
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setstate(row[2]);
                vec.push_back(user);
                
            }
            // 由于用了 MYSQL_RES * res = mysql.query(sql); 需要释放资源
            mysql_free_result(res);    // mysql_free_result调用进行资源释放
            return vec;
        }
        // 查询失败
    }
    

    return vec;
}