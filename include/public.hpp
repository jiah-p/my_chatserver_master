#ifndef PUBLIC_H
#define PUBLIC_H


// 客户端和服务端公共的标识信息
// 实现状态到事件的映射
enum MsgType{
    LOGIN_MSG = 1,   // 登陆消息
    LOGIN_MSG_ACK,   // 登陆响应消息
    LOGINOUT_MSG, // 注销消息
    REG_MSG,         // 注册消息 
    REG_MSG_ACK,      // 注册响应消息
    ONE_CHAT_MSG,    // 点对点聊天消息
    ADD_FRIEND_MSG,   // 添加好友信息

    CREATE_GROUP_MSG,   // 创建群组
    ADD_GROUP_MSG,     // 加入群组
    GROUP_CHAT_MSG    // 群聊天
};

#endif 