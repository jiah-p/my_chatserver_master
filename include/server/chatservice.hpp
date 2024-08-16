#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include "json.hpp"
#include <string>
#include "public.hpp"
#include <iostream>
#include <muduo/base/Logging.h> // 打印错误日志
#include <string>
// #include <map>
#include <vector>
#include "chatservice.hpp"
// #include <pthread.h>
#include <mutex>
// #include <memory>  
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"

using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;
using namespace std;

// 通过using 取别名
using MsgHandler = std::function<void(const TcpConnectionPtr & conn, json& js, Timestamp time)>;  // 通过 function 实现统一接口
// 聊天服务器的业务模块
class ChatService{
public:
    // 设计为单例模式
    static ChatService* getinstance(void);
    // 处理登陆业务
    void login(const TcpConnectionPtr & conn, json& js , Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr & conn, json& js , Timestamp time);
    // 一对一聊天业务 
    void onechat(const TcpConnectionPtr & conn, json& js , Timestamp time);
    // 获取对应的处理器
    MsgHandler getHandler(int msgid);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr & conn);
    // 重置服务器 当服务器异常断开后 链接信息需要更新
    void reset();

    // 添加好友业务
    void addfriend(const TcpConnectionPtr & conn, json& js , Timestamp time);

    // 添加创建群组业务
    void creategroup(const TcpConnectionPtr & conn, json& js , Timestamp time);
    // 添加加入群组业务 
    void addgroup(const TcpConnectionPtr & conn, json& js , Timestamp time);
    // 添加群组聊天业务
    void groupchat(const TcpConnectionPtr & conn, json& js , Timestamp time);



private:
    // 消息处理器的映射关系表 消息 -> 业务
    unordered_map<int, MsgHandler> _msgHanlderMap;

    ChatService();
    
    // 数据操作类对象 
    

    UserModel _usermodel;
 
    // 存储在线用户的通信连接信息  需要保证线程安全
    // 互斥锁 
    mutex _userConnMapMutex;
    // 用户ID -> tcp链接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 离线
    OfflineMsgModel _offlinemsgmodel;

    FriendModel _friendmodel;   // 好友类的操作接口
 
    GroupModel _groupmodel; // 群组类的操作接口
};





#endif