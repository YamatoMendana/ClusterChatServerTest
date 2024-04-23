#ifndef USER_H
#define USER_H

#include <string>

using namespace std;

class User
{
public:
    User(int id=-1,string name="",string pwd="",string state="offline")
    {
        this->id = id;
        this->name = name;
        this->password = password;
        this->state = state;
    }

    inline void setId(int id)
    {
        this->id = id;
    }
    inline void setName(string name)
    {
        this->name = name;
    }
    inline void setPwd(string pwd)
    {
        this->password = pwd;
    }
    inline void setState(string state)
    {
        this->state = state;
    }

    inline int getId()
    {
        return this->id;
    }
    inline string getName()
    {
        return this->name;
    }
    inline string getPwd()
    {
        return this->password;
    }
    inline string getState()
    {
        return this->state;
    }

private:
    int id;
    string name;
    string password;
    string state;
};

#endif