#include "offlineMsgModel.h"
#include "sqlserver.h"

void OfflineMsgModel::insert(int userid, string msg)
{
    char strsql[1024] = {0};
    sprintf(strsql,"insert  into OfflineMessage(userid,message) values(%d,'%s')",userid,msg.c_str());

    cMySQL mysql;

    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        mysql.Update(strsql);
    }
}

void OfflineMsgModel::remove(int userid)
{
    char strsql[1024] = {0};
    sprintf(strsql,"delete from OfflineMessage where userid = %d",userid);

    cMySQL mysql;
    if(mysql.Connect())
    {
        mysql.useDatabase("chat");
        mysql.Update(strsql);
    }
}

vector<string> OfflineMsgModel::query(int userid)
{
    char strsql[1024] = {0};
    sprintf(strsql,"select * from OfflineMessage where userid = %d order by create_time asc"
        ,userid);

    cMySQL mysql;
    vector<string> vec;
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
                    vec.push_back(root[i]["message"].asString());
                }
                
            }
            

        }
        
    }
    return vec;
}
