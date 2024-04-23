#ifndef GROUP_H
#define GROUP_H

#include "groupUser.h"
#include <string>
#include <vector>

using namespace std;

class Group
{
public:
    Group(int id=-1,string name="",string desc="")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }
    

    inline void setId(int id)
    {
        this->id = id;
    }
    inline void setName(string name)
    {
        this->name = name;
    }
    inline void setDesc(string desc)
    {
        this->desc = desc;
    } 

    inline int getId()
    {
        return this->id;
    }

    inline string getName()
    {
        return this->name;
    }

    inline string getDesc()
    {
        return this->desc;
    }

    vector<GroupUser>& getUsers()
    {
        return this->users;
    }

private:
    int id;
    string name;
    string desc;

    vector<GroupUser> users;
};








#endif 