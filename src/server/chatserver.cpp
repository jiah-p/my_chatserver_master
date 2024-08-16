#include "chatserver.hpp"
#include "chatservice.hpp"

ChatServer::ChatServer(EventLoop* loop,    // 事件循环 Reactor
        const InetAddress& listenAddr,  // IP + Port
        const string& nameArg       // 服务器名称
        ):_server(loop, listenAddr, nameArg),_loop(loop){
            // 给服务器注册 连接事件
            _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

            //  给服务器注册 用户读写事件 回调 
            _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
            
            // 设置线程数量  1 个 io 线程， 3个worker线程  阿里云服务器核心为2
            _server.setThreadNum(4);
    }

// 启动服务
void ChatServer::start(){
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            cout << conn -> peerAddress().toIpPort() << "->" << 
            conn->localAddress().toIpPort() <<  "state:online " <<endl;
        }
        else{
            // 连接断开 
            ChatService::getinstance()->clientCloseException(conn);
            cout << "state:offline " <<endl;
            conn->shutdown();   // 断开连接 等价于 close(fd)
            // _loop->quit();  事件循环退出，epoll退出. 如果这个退出，这个服务器就结束了
        }
        
    }
//  Buffer 封装了缓冲区的类
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time){
    string buf = buffer->retrieveAllAsString();  // 将消息转成string
    // json 数据的反序列化
    json js = json::parse(buf);

    // 解耦网络和业务模块的代码
    // 根据不同的状态位调用不同的服务
    // 事件分发器
    // js[msgid]  -> 事件处理器 hanlder

    // 面向抽象基类编程
    // 回调函数 -> 服务
    // json对象 get<int>() 通过 int 进行实例化 然后进行强转，需要转哪个就传哪个参数
    auto msgHandler = ChatService::getinstance()->getHandler(js["msgid"].get<int>());

    // 根据不同的msgHandler 进行事件派发
    // 回调消息绑定的事件处理器，来执行相应的业务处理器
    msgHandler(conn, js , time);

}