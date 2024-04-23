#ifndef GROUPUSER_H
#define GROUPUSER_H

#include <string>

#include "user.h"

using namespace std;

class GroupUser:public User
{
public:
    inline void setRole(string role)
    {
        this->role = role;
    }

    inline string getRole()
    {
        return this->role;
    }


private:
    string role;

};





#endif