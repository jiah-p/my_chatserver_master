#include  "chatservice.hpp"

ChatService * ChatService::getinstance(void){
    static ChatService service;
    return &service;
}

ChatService::ChatService(){
    // 建立 消息到 事件处理器的绑定
    // log 业务
    _msgHanlderMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    // reg 业务
    _msgHanlderMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    // one_chat 业务
    _msgHanlderMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::onechat, this, _1, _2, _3)});
    // addfriend 添加好友业务
    // ADD_FRIEND_MSG
    _msgHanlderMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addfriend, this, _1, _2, _3)});
 
    // CREATE_GROUP_MSG
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3)});
    // ADD_GROUP_MSG
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    // GROUP_CHAT_MSG
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}

void  ChatService::reset(){
    // 目前：把所有用户状态设置为offline
    _usermodel.resetstate();
}

// 获取消息对应的状态位
MsgHandler ChatService::getHandler(int msgid){
    if(!_msgHanlderMap.count(msgid))
    {
        // 可以提供一些默认 业务进行扩展 
        // 目前返回一个默认的处理器
        return [=](const TcpConnectionPtr & conn, json& js, Timestamp time){
            LOG_ERROR << "msgid: " << msgid << "can not find handler!";
        };
    }
    return _msgHanlderMap[msgid];
}

// 处理登陆业务 需要输入信息 id password
void ChatService::login(const TcpConnectionPtr & conn, json& js, Timestamp time){
    int id = js["id"].get<int>();   // 注意这个点
    string password = js["password"];

    User user = _usermodel.query(id);
    if(user.getid() != -1 && user.getpassword() == password){
        if(user.getstate() == "online"){
            // 说明该用户已经登陆
            // 该用户不存在
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errmsg"] = "this account already online, please change one another.";
            response["erro"] = 2;    // 响应回复标志位 正常
        
            // json 序列化后发送
            conn->send(response.dump());
        }
        else{
             // 登陆成功  需要记录连接信息
            // explicit lock_guard(mutex_type& __m) : _M_device(__m)
            // { _M_device.lock(); }
            {     // 利用局部变量 缩小临界区的范围
            lock_guard<mutex>lock(_userConnMapMutex);
            _userConnMap.insert({id,conn});   // 这是个互斥量 目前设置的3个工作线程 所以需要互斥访问 mutex
            }
            
             // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 

            // 更新用户状态信息 state offline -> online
            user.setstate("online");
            _usermodel.updatestate(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["id"] = user.getid();
            response["erro"] = 0;    // 响应回复标志位 正常
            response["name"] = user.getname();

            // 查询该用户是否有离线消息
            vector<string> vec = _offlinemsgmodel.query(id);
            if(!vec.empty()){
                // 读取该用户的离线消息后，需要进行删除
                response["offlinemsg"] = vec;
                _offlinemsgmodel.remove(id);
            }
            // 查询该用户的好友信息并返回
            vector<User> userVec = _friendmodel.query(id);
            if(!userVec.empty()){
                vector<string> vec2;
                for(auto& user: userVec){
                    json js;
                    js["id"] = user.getid();
                    js["name"] = user.getname();
                    js["state"] = user.getstate();
                    vec2.push_back(js.dump());
                }

                response["friends"] = vec2;
            }

            // json 序列化后发送
            conn->send(response.dump());

        }  
    }
    else{
        
        // 该用户不存在 用户存在但是密码错误，或者用户id错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errmsg"] = "id or password error.";
        response["erro"] = 1;    // 响应回复标志位 正常
    
        // json 序列化后发送
        conn->send(response.dump());
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr & conn, json& js, Timestamp time){
    User user;
    user.setname(js["name"]);
    user.setpassword(js["password"]);
    if(_usermodel.insert(user)){
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["id"] = user.getid();
        response["erro"] = 0;    // 响应回复标志位 正常
        // json 序列化后发送
        conn->send(response.dump());
    }
    // 注册失败
    else{
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["erro"] = 1;    // 响应回复标志位 错误
        // json 序列化后发送
        conn->send(response.dump());
    
    }
}

// 处理客户端异常退出
// 用户状态更新 offline
// 连接信息删除
void ChatService::clientCloseException(const TcpConnectionPtr & conn){
    User user;
    {   // 缩小锁的粒度
        lock_guard<mutex>lock(_userConnMapMutex);
        
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); it++){
            if(it->second == conn){
                // 删除链接信息
                user.setid(it->first);
                _userConnMap.erase(it);
                break;
            }
        }   
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    // 更新用户的状态信息
    if(user.getid()!= -1){
        user.setstate("offline");
        _usermodel.updatestate(user);
    }
}


/*
    聊天格式
    msgid
    id
    name
    to
    msg
*/
// 一对一聊天业务 
void ChatService::onechat(const TcpConnectionPtr & conn, json& js , Timestamp time){
    int toid = js["to"].get<int>();

    // 线程安全 it的必须在锁的范围内
    {
        lock_guard<mutex>lock(_userConnMapMutex);
        auto it = _userConnMap.find(toid);
        if(it!= _userConnMap.end()){
            // 说明对方在线
            // 服务器主动推送消息给 to用户
            // 如果使用中文进行交流，json会解析出错 需要更换解析方式
            
            it->second->send(js.dump());
            return;
        }
        // 查询toid是否在线 
        User user = _userModel.query(toid);
        if (user.getstate() == "online")
        {
            _redis.publish(toid, js.dump());
            return;
        }

        // 说明对方不在线 存储离线消息
        _offlinemsgmodel.insert(toid, js.dump());
        
    }
}

// 添加好友业务 msgid id friendid 
void ChatService::addfriend(const TcpConnectionPtr & conn, json& js , Timestamp time){
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 可以进行验证 friendid是否存在
    // 存储好友信息
    _friendmodel.insert(userid, friendid);


}

// 添加创建群组业务
void ChatService::creategroup(const TcpConnectionPtr & conn, json& js , Timestamp time){
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 将新创建的群加入数据库
    // 将创建人加入gruop表,并设置 creator属性
    Group group;
    group.setname(name);
    group.setdesc(desc);
    if(_groupmodel.creategroup(group)){
        _groupmodel.addgroup(userid, group.getid(), 'creator');
    }

    // 可以考虑 用 ACK 用 json 进行响应
}
// 添加加入群组业务 
void ChatService::addgroup(const TcpConnectionPtr & conn, json& js , Timestamp time){
    int userid = js["id"].get<int>();
    string groupid = js["groupid"].get<int>();

    // 将新创建的群加入数据库
    _groupmodel.addgroup(userid,groupid , 'normal');
    // 可以考虑 用 ACK 用 json 进行响应
}

// 添加群组聊天业务
void ChatService::groupchat(const TcpConnectionPtr & conn, json& js , Timestamp time){
    int userid = js["id"].get<int>();
    string groupid = js["groupid"].get<int>();
    // 存储了组内所有人的id信息。需要访问连接信息，注意线程安全
    vector<int> useridvec = _groupmodel.querygroupusers(userid, groupid);
    // 遍历所有在线的人员，进行消息的发送
    {
        lock_guard<mutex>lock(_userConnMapMutex); // 加锁循环放在外面
        for(int id: useridvec){
            // 发送人在线 则直接发送
            auto it = _userConnMap.find(id);
            if( auto != _userConnMap.end()){
                // redis转发
                
                it->second->send(js.dump());
                continue;
            } 
            
            User user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
            _offlinemsgmodel.insert(id, js.dump());
            }
        }
    }
}


// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}