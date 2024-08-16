#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

string func1(){
    json js;
    js["msg_type"] = 2 ;
    js["from"] = "zhagn san " ;
    js["to"] = "li si" ;
    js["msg"] = "hello world" ;

    string sendBuf = js.dump();

    // cout << sendBuf.c_str() << endl;

    return sendBuf;
}

// 序列化更加复杂内容
// 可以多层嵌套 key fileds.. value
// 可以适配C++中的容器，直接序列化成容器

string func2(){
    json js;

    js["id"] = {1,2,3,4,5};
    js["name"] = "zhang san";
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello liu";

    // 等价于
    js["msgequal"] = {{"zhang san", "hello world"},{"liu shuo", "hello liu"}};

    string sendbuf = js.dump();   // 要用string

    // cout <<  js << endl;

    return sendbuf;
}

string func3(){
    json js;
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(3);
    vec.push_back(5);

    js["list"] = vec;

    map<int, string>mp;
    mp[1] = "华山";
    mp.insert(make_pair(2,"黄山"));
    mp.insert(pair<int, string>(3,"黄山"));

    js["path"] = mp;

    string sendbuf = js.dump();

    return sendbuf;
}

int main(){

    string recvbuf = func1();
    // 数据的反序列化，json 字符串 -> 反序列化  可以看作容器 ，返回一个string
    json buf = json::parse(recvbuf);
    cout << buf << endl;
    cout << buf["msg_type"] << endl;  // 会保留原始数据的类型 这个位置是int
    cout << buf["from"] << endl;
    cout << buf["to"] << endl;
    cout << buf["msg"] << endl;

    recvbuf  = func2();
    buf = json::parse(recvbuf);
    cout << buf["id"][1] << endl;  // 会保留原始数据的类型 这个位置是int
    cout << buf["name"] << endl;
    cout << buf["msg"] << endl;
    cout << buf["msgequal"] << endl;

    recvbuf = func3();
    buf = json::parse(recvbuf);

    vector<int>rec_vec = buf["list"];   // json对象里的数组类型，直接放入vector容器中
    map<int, string>rec_map = buf["path"]; 

    for(auto i: rec_vec)
        cout << i  << ' ';
    cout << endl;
     for(auto i: rec_map)
        cout << i.first << ' '<< i.second ;
    cout << endl;

}
