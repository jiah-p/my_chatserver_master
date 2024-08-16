#include  "offlinemessagemodel.hpp"
#include "db.h"

// 插入户离线消息
void OfflineMsgModel::insert(int userid, string msg){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, %s)",
    userid, msg.c_str());

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }

    return ;
}
// 移除用户离线消息
void OfflineMsgModel::remove(int userid){
    char sql[1024] = {0};
    sprintf(sql, "delete from  offlinemessage where userid=%d",
    userid);

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }

    return ;
}
// 查询用户离线消息
vector<string> OfflineMsgModel::query(int userid){
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d",
    userid);

    vector<string>vec;
    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res  = mysql.query(sql);
        if(res!=nullptr){
            MYSQL_ROW  row;
            // 将 userid 的所有离线消息存入vec
            while ((row  = mysql_fetch_row(res)) != nullptr)
            {
               vec.push_back(row[0]);
            }
            
            mysql_free_result(res);
            return vec;
        }
    }

    return vec;
}