
#include "usermodel.hpp"
#include "db.h"
#include <iostream>
#include <mysql/mysql.h>

using namespace std;
/*
User 表
id int PRIMARY KEY, AUTO_INCRMENT
name varchar(50)  NOT NULL, UNIQUE_FLAG
password varchar(50)  NOT NULL
state ENUM('online','offline') DEFAULT 'offline'
*/

//  User表的插入方法
bool UserModel::insert(User& user){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values(%s, %s, %s)",
    user.getname().c_str(), user.getpassword().c_str(), user.getstate().c_str());

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            // 获取插入成功的用户的自增主键
            // uint64_t STDCALL mysql_insert_id(MYSQL *mysql);
            user.setid(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}


//  User表的查询方法 根据提供的ID 进行表的查询
User UserModel::query(int id){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "select id, name, password, state from user where id = %d",
    id);

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES * res = mysql.query(sql);
        if(res != nullptr){
            // 查询成功
            // uint64_t STDCALL mysql_insert_id(MYSQL *mysql);
            MYSQL_ROW row = mysql_fetch_row(res);   // 得到查询的数据一行
            if(row != nullptr){
                User user;
                // 根据表的结构和查询的语句分类得到数据 
                // 得到的是字符串
                user.setid(atoi(row[0]));
                user.setname(row[1]);
                user.setpassword(row[2]);
                user.setstate(row[3]);

                // 由于用了 MYSQL_RES * res = mysql.query(sql); 需要释放资源
                mysql_free_result(res);    // mysql_free_result调用进行资源释放
                return user;
            }
        }
        // 查询失败
        return User();
    }
    

    return false;
}

bool UserModel::updatestate(User& user){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = {0};
    sprintf(sql, "update user set state = %s where id = %d ",user.getstate().c_str(), user.getid());

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
       
            if(mysql.update(sql)){
                
                return true;
            }
    }
    
    return false;
}

 // 重置用户状态信息
void UserModel::resetstate(){
    // 组装sql语句便于插入mysql
    // 注意表的设计
    char sql[1024] = "update user set state = 'offline' where state = 'online' ";

    // 创建Mysql对象并连接操作
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);    
    }
    
    return ;
}