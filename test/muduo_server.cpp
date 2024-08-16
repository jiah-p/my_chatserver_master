/*
    muduo网络库作用：
        TcpServer：用于编写服务器程序
        TcpClient：用于编写客户端程序

    epoll + 线程池
    好处：将网络IO代码和业务代码分开
        用户的连接和短开
        用户的可读写事件
*/

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>

//  加载命名空间
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

// 基于 muduo 网络库的开发服务器程序环境测试
// muduo 库封装了 线程池和日志系统 
// muduo 网络设计：reactors in threads  - one loop per thread
// nginx 服务器设计：reactors in process - one loop per process  通过一把 accept锁，解决多个 Reactors的 惊群现象
// 组合TcpServer 对象
// 创建EventLoop 事件循环对象的指针
// 明确TcpServer 构造函数需要的参数，输出 ChatServer 构造函数
// 给在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写事件的回调函数
// 设置合适的服务端线程数量，muduo库会自己分配IO线程和worker线程

// 定义 SRC_LIST 变量 表示包含当前目录下所有文件
// aux_source_directory(. SRC_LIST)
// 指定生成可执行文件   -o
// add_executable(ChatServer ${SRC_LIST})
// 指定可执行文件链接时需要依赖的库文件 -l
// target_link_libraries(ChatServer muduo_net muduo_base pthread)

// 顶层
// 配置编译器版本
// cmake_minimum_required(VERSION 3.0)
// project(chat)

// 配置编译选项 -g
// set(CMAKE_CXX_FLAGS $(CMAKE_CXX_FLAGS) -g)

// 配置最终的可执行文件的输出路径 
// set(EXECUTABLE_OUTPUT_PATH ${{PROJECT_SOURCE_DIR}}/bin)

// 加载头文件搜索路径 -I
// include_directories( ${{PROJECT_SOURCE_DIR}}/include)
// include_directories( ${{PROJECT_SOURCE_DIR}}/include/server)

// 加载子目录  -L
// add_subdirectory(src)

// vscode没有编译环境，所以一般还是用的 g++

// InetAddress

// private:
//   union
//   {
//     struct sockaddr_in addr_;
//     struct sockaddr_in6 addr6_;
//   };

  
class ChatServer{
public:
    // ChatServer 构造函数
    ChatServer(EventLoop* loop,    // 事件循环 Reactor
        const InetAddress& listenAddr,  // IP + Port
        const string& nameArg       // 服务器名称
        ):_server(loop, listenAddr, nameArg),_loop(loop){
            //  服务器 和 用户相关代码
            //  给服务器注册用户连接和创建和断开 回调    ---  回调函数 
            /// Set connection callback.
            /// Not thread safe.
            // typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;   
            // bind 函数适配器 可以改造函数 适配不同的情况 _1占位符 因为需要this指针所以绑定了this
            _server.setConnectionCallback(std::bind(&onConnection, this,  _1));  

            /// Set message callback.
            /// Not thread safe.
            //消息回调  onMessage 有三个参数，所以三个占位符 通过this访问当前对象成员对象
            //  给服务器注册 用户读写事件 回调 
            _server.setMessageCallback(std::bind(&onMessage, this,  _1, _2, _3));  
            
            // 设置线程数量  1 个 io 线程， 3个worker线程
            _server.setThreadNum(4);
    }
    // 开启事件循环
    void start(){
        _server.start();
    }
    
private:
    // 专门用处理用户的连接的创建和断开 epoll listenf accept
    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            cout << conn -> peerAddress().toIpPort() << "->" << 
            conn->localAddress().toIpPort() <<  "state:online " <<endl;
        }
        else{
            // 连接断开 
            cout << "state:offline " <<endl;
            conn->shutdown();   // 断开连接 等价于 close(fd)
            // _loop->quit();  事件循环退出，epoll退出. 如果这个退出，这个服务器就结束了
        }
        
    }
    //  Buffer 封装了缓冲区的类
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time){
        string buf = buffer->retrieveAllAsString();  // 将消息转成string
        cout << "recv data:" << buf << "time: " << time.toString();
        conn->send(buf);
    }

    // the data has been read to (buf, len)
    // typedef std::function<void (const TcpConnectionPtr&,    // 连接
    //                         Buffer*,                        // 缓冲区
    //                         Timestamp)> MessageCallback;    // 接收到数据的时间信息

    TcpServer _server;
    //TcpServer(EventLoop* loop, const InetAddress& listenAddr);

    // TcpServer(EventLoop* loop,  // 事件循环
    //         const InetAddress& listenAddr,   // 监听地址和端口号
    //         const string& nameArg,           // tcpserver 名称
    //         Option option = kNoReusePort);

    EventLoop* _loop; // Reactor epoll
    ///
    /// Reactor, at most one per thread.
    ///
    /// This is an interface class, so don't expose too much details.

};

int main(){
    EventLoop loop;   // 设置 epoll
    InetAddress addr("127.0.0.1", 6000);

    ChatServer chatserver(&loop, addr, "chatserver");

    chatserver.start();     // 启动 TCP服务器， listen 然后通过epoll_ctl 向 epoll 注册事件
    loop.loop();        // epoll_wait 以阻塞方式等待新用户连接，已连接用户的读写事件等

    return 0;
}