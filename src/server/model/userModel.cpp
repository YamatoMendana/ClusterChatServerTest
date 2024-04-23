#include "userModel.h"

#include "sqlserver.h"
#include <iostream>

void UserModel::resetState()
{
    char strsql[1024]={0};
    sprintf(strsql,"update User set state='offline' where state = 'online'");

    cMySQL mysql;
    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        mysql.Update(strsql);
  
    }
}

bool UserModel::insert(User &user)
{
    char strsql[1024]={0};
    sprintf(strsql,"insert into User(name,password,state) values('%s','%s','%s');"
                ,user.getName().c_str(),user.getPwd().c_str(),user.getState().c_str());

    cMySQL mysql;
    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        if(mysql.Update(strsql))
        {
            user.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }
    
    return false;
}

User UserModel::query(int id)
{
    char strsql[1024] ={0};
    sprintf(strsql,"select * from User where id = %d",id);
    
    cMySQL mysql;
    if(mysql.Connect())
    {
        string strRet = mysql.Query(strsql);

        Json::Reader reader;
        Json::Value root;
        bool bret = reader.parse(strRet,root);
        if(bret)
        {
            User user;
            int size = root.size();
            for(int i = 0; i < size;i++)
            {
                user.setId(root[i]["id"].asInt());
                user.setName(root[i]["name"].asString());
                user.setPwd(root[i]["password"].asString());
                user.setState(root[i]["state"].asString());
            }
            

            return user;
        }

    }
    
    return User();
}

bool UserModel::updateState(User user)
{
    char strsql[1024] ={0};
    sprintf(strsql,"update User set state = '%s' where id = %d ",user.getState().c_str(),user.getId());

    cMySQL mysql;
    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        if(mysql.Update(strsql))
        {
            user.setId(mysql_insert_id(mysql.getConnection()));

            return true;
        }
    }

    return false;
}

// bool UserModel::insert(User& user,SQL_Base* sql)
// {
//     char strsql[1024]={0};
//     sprintf(strsql,"insert into User(name,password,state) values('%s','%s','%s');"
//                 ,user.getName().c_str(),user.getPwd().c_str(),user.getState().c_str());

//     if(sql->Connect())
//     {
//         if(sql->Update(strsql))
//         {
//             user.setId(sql->get_insert_id());
//             return true;
//         }
//     }
    
//     return false;
// }

