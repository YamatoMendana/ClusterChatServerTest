#include "friendmodel.h"
#include "sqlserver.h"
#include "user.h"

using namespace std;

void FriendModel::insert(int userid, int friendid)
{
    char strsql[1024] = {0};
    sprintf(strsql,"insert  into Friend (userid,friendid) values(%d,%d)",userid,friendid);

    cMySQL mysql;

    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        mysql.Update(strsql);
    }

}

vector<User> FriendModel::query(int userid)
{
    char strsql[1024] = {0};
    sprintf(strsql
        ,"select a.id as id,a.name as name,a.state as state from User a inner join Friend b on b.friendid=a.id where b.userid = %d"
        ,userid);

    cMySQL mysql;
    vector<User> vec;
    if(mysql.Connect())
    {
        string strRet = mysql.Query(strsql);
        if(!strRet.empty())
        {
            Json::Reader reader;
            Json::Value root;
            bool bret = reader.parse(strRet,root);
            if(bret)
            {
                int size = root.size();
                for(int i = 0; i < size;i++)
                {
                    User user;
                    user.setId(root[i]["id"].asInt());
                    user.setName(root[i]["name"].asString());
                    user.setState(root[i]["state"].asString());
                    vec.push_back(user);
                }
                return vec;
            }
            

        }
        
    }
    return vec;
}
