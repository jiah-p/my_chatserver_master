#include "chatserver.hpp"
#include "chatservice.hpp"
#include <iostream>
#include <signal.h>

using namespace std;

// 捕获 SIGINT 信号
void resthandler(int){
    // 重置客户端，将客户端都设置为离线状态
    ChatService::getinstance()->reset();
}

int main(int argc, char ** argv){
    if(argc < 3){
        cout << " Command invaild: input ./Chatserver 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 基于传入的 命令行参数得到IP和Port
    char * ip= argv[1];
    uint16_t port = atoi(argv[2]);   // convert a string to int

    // 设置信号捕捉函数
    signal(SIGINT, resthandler);

    // 绑定ip和端口
    EventLoop loop;

    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;

}


