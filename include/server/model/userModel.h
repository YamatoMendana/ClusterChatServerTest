#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.h"

class UserModel
{
public:
    void resetState();

    bool insert(User& user);
    // bool insert(User& user,SQL_Base* sql);

    User query(int id);

    bool updateState(User user);

private:



};





#endif