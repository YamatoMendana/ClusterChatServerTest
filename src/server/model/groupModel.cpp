#include "groupModel.h"
#include "sqlserver.h"

bool GroupModel::createGroup(Group &group)
{
    char strsql[1024]={0};

    sprintf(strsql,"insert into AllGroup(groupname,groupdesc) values('%s','%s')",
        group.getName().c_str(),group.getDesc().c_str());

    cMySQL mysql;
    if(mysql.Connect())
    {
        if(mysql.Update(strsql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }



    return false;
}

bool GroupModel::addGroup(int userid, int groupid, string role)
{
    char strsql[1024]={0};

    sprintf(strsql,"insert into GroupUser values(%d,%d,'%s')",
        userid,groupid,role.c_str());

    bool ret = false;
    cMySQL mysql;
    if(mysql.Connect())
    {
        ret = mysql.Update(strsql);
    }  

    return ret;

}

vector<Group> GroupModel::queryGroups(int userid)
{
    char strsql[1024]={0};

    sprintf(strsql,"insert into a.id as id, a.groupname as groupname,a.groupdesc as groupdesc from AllGroup a inner join \
                GroupUser b on a.id = b.groupid where b.userid=%d",userid);

    vector<Group> groupVec;

    bool ret = false;
    cMySQL mysql;
    if(mysql.Connect())
    {
        string strRet = mysql.Query(strsql);

        Json::Reader reader;
        Json::Value root;
        bool bret = reader.parse(strRet,root);
        if(bret)
        {
            Group group;
            int size = root.size();
            for(int i = 0; i < size;i++)
            {
                group.setId(root[i]["id"].asInt());
                group.setName(root[i]["groupname"].asString());
                group.setDesc(root[i]["groupdesc"].asString());

                groupVec.push_back(group);
            }
        }

        for(Group& group:groupVec)
        {
            sprintf(strsql,"select a.id as id,a.name as name,a.state as state,b.grouprole as grouprole \
                from user a inner join  GroupUser b on b.userid = a.id where b.groupid = %d",group.getId());
            bool bret = reader.parse(strRet,root);
            if(bret)
            {
                GroupUser guser;
                int size = root.size();
                for(int i = 0; i < size;i++)
                {
                    guser.setId(root[i]["id"].asInt());
                    guser.setName(root[i]["groupname"].asString());
                    guser.setState(root[i]["groupdesc"].asString());
                    guser.setRole(root[i]["grouprole"].asString());
                    group.getUsers().push_back(guser);
                }
                
            }
                
        }
    }  



    return groupVec;
}

vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char strsql[1024]={0};

    sprintf(strsql,"select userid from GroupUser where groupid = %d and userid != %d",
        groupid,userid);

    vector<int> idVec;
    bool ret = false;
    cMySQL mysql;
    if(mysql.Connect())
    {
        string strRet = mysql.Query(strsql);

        Json::Reader reader;
        Json::Value root;
        bool bret = reader.parse(strRet,root);
        if(bret)
        {
            int size = root.size();
            for(int i = 0; i < size;i++)
            {
                idVec.push_back(root[i]["userid"].asInt());
            }
        }   
    }  

    return idVec;
}
