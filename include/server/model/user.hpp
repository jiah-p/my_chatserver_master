#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User{
public:
    User(int id = -1, string name= "", string password="", string state = "offline"):
        _name(name),_id(id),_password(password),_state(state){}

    void setid(uint64_t id){_id = id;}
    void setname(string name){_name = name;}
    void setpassword(string password){_password = password;}
    void setstate(string state){_state = state;}

    int getid(){return _id;}
    string getname(){return _name;}
    string getpassword(){return _password;}
    string getstate(){return _state;}

private:
    string _name;
    uint64_t _id;
    string _password;
    string _state;
};

#endif