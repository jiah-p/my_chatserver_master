#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include "json.hpp"

using json = nlohmann::json;
using namespace muduo;
using namespace muduo::net;
using namespace std;

// 聊天服务器 主类
class ChatServer{
public:
    // ChatServer 构造函数
    ChatServer(EventLoop* loop,    // 事件循环 Reactor
        const InetAddress& listenAddr,  // IP + Port
        const string& nameArg       // 服务器名称
        );
// 开启事件循环
    void start();
    
private:
    // 专门用处理用户的连接的创建和断开 epoll listenf accept
    // 处理连接相关回调函数
    void onConnection( TcpConnectionPtr& conn);
    //  Buffer 封装了缓冲区的类
    // 上报读写事件相关信息的回调函数
    void onMessage( TcpConnectionPtr& conn, Buffer* buffer, Timestamp time);


 
    TcpServer _server;  // 实现服务器功能的类对象  has a 在muduo的框架上，实现自己的聊天服务器，集中开发业务，解耦网络

    EventLoop* _loop;  // 指向事件循环的指针  /// Reactor, at most one per thread.
};

#endif