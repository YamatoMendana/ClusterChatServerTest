#include "chatservice.h"
#include "../public/public.h"
#include <muduo/base/Logging.h>
#include <string>
#include <vector>


using namespace std;
using namespace muduo;

chatservice *chatservice::instance()
{
    static chatservice service;
    return &service;
}

chatservice::chatservice()
{
    m_msghandlerMap.insert({LOGIN_MSG,std::bind(&chatservice::login,this,_1,_2,_3)});
    m_msghandlerMap.insert({REGISTER_MSG,std::bind(&chatservice::reg,this,_1,_2,_3)});
     m_msghandlerMap.insert({LOGIN_OUT_MSG,std::bind(&chatservice::loginout,this,_1,_2,_3)});
    m_msghandlerMap.insert({CHAT1BY1_MSG,std::bind(&chatservice::chat1by1,this,_1,_2,_3)});
    m_msghandlerMap.insert({ADD_FRIEND_MSG,std::bind(&chatservice::addFriend,this,_1,_2,_3)});
    m_msghandlerMap.insert({CREATE_GROUP_MSG,std::bind(&chatservice::createGroup,this,_1,_2,_3)});
    m_msghandlerMap.insert({ADD_GROUP_MSG,std::bind(&chatservice::addGroup,this,_1,_2,_3)});
    m_msghandlerMap.insert({GROUP_CHAT_MSG,std::bind(&chatservice::groupChat,this,_1,_2,_3)});

    if(m_redis.connect())
    {
        m_redis.init_notify_handler(std::bind(&chatservice::handleRedisSubcribeMessage,this,_1,_2));
    }

}

void chatservice::reset()
{
    m_userModel.resetState();

}


void chatservice::login(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int id = js["id"].asInt();
    string pwd = js["password"].asString();

    User user = m_userModel.query(id);
    if(user.getId()== id && user.getPwd() == pwd)
    {
        if(user.getState() == "online")
        {
            Json::Value response;
            response["msgid"] = REGISTER_MSG_ACK;
            response["error"] = 2;
            response["errorMsg"] = "该账号已登录，请重新输入账号";
            Json::StreamWriterBuilder writer;
            string respStr = Json::writeString(writer,response);
            conn->send(respStr.c_str());
        }
        else
        {
            Json::Value response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["error"] = 0;
            response["errorMsg"] = "";
            response["id"] = user.getId();
            response["name"] = user.getName();

            //查询离线信息
            vector<string> vec = m_offlineMsgModel.query(id);
            if(!vec.empty())
            {
                for(int i = 0 ;i <vec.size();i++)
                {
                    response["offlineMsg"].append(vec[i]);
                }

                m_offlineMsgModel.remove(id);

            }

            //查询好友信息
            vector<User> userVec = m_friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string> friVec;
                for(User& user: userVec)
                {
                    Json::Value friJs;
                    friJs["id"] = user.getId();
                    friJs["name"] = user.getName();
                    friJs["state"] = user.getState();
                    response["friends"].append(friJs);
                }
                
            }
            {
                lock_guard<mutex> lock(m_connMutex);
                m_userConnMap.insert({id,conn});
            }
            m_redis.subscribe(id);

            user.setState("online");
            m_userModel.updateState(user);

            Json::StreamWriterBuilder writer;
            string respStr = Json::writeString(writer,response);
            LOG_INFO<<respStr;
            conn->send(respStr.c_str());
        }
        
    }
    else
    {
        Json::Value response;
        response["msgid"] = REGISTER_MSG_ACK;
        response["error"] = 1;
        response["errorMsg"] = "密码不正确";

        Json::StreamWriterBuilder writer;
        string respStr = Json::writeString(writer,response);
        conn->send(respStr.c_str());
         
    }
}

void chatservice::reg(const TcpConnectionPtr &conn, Json::Value js, Timestamp time)
{
    string name = js["name"].asString();
    string pwd = js["password"].asString();

    User user;
    user.setName(name);
    user.setPwd(pwd);


    bool state = m_userModel.insert(user);
    if(state)
    {
        LOG_INFO<<"reg success";
        Json::Value response;
        response["msgid"] = REGISTER_MSG_ACK;
        response["error"] = 0;
        response["errorMsg"] = "";
        response["id"] = user.getId();
        response["name"] = user.getName();

        Json::StreamWriterBuilder writer;
        string respStr = Json::writeString(writer,response);
        conn->send(respStr.c_str());
    }
    else
    {
        LOG_INFO<<"reg failed";
        Json::Value response;
        response["msgid"] = REGISTER_MSG_ACK;
        response["error"] = 1;
        response["errorMsg"] = "注册错误"; 

        Json::StreamWriterBuilder writer;
        string respStr = Json::writeString(writer,response);
        conn->send(respStr.c_str());
    }

}

void chatservice::addFriend(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int userid = js["id"].asInt();
    int friendid = js["friendid"].asInt();

    m_friendModel.insert(userid,friendid);

}

void chatservice::createGroup(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int userid = js["id"].asInt();
    string name = js["groupname"].asString();
    string desc = js["groupdesc"].asString();

    Group group(-1,name,desc);
    if(m_groupModel.createGroup(group))
    {
        m_groupModel.addGroup(userid,group.getId(),"creator");
    }

}

void chatservice::addGroup(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int userid = js["id"].asInt();
    int groupid = js["groupid"].asInt();

    m_groupModel.addGroup(userid,groupid,"normal");

}

void chatservice::groupChat(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int userid = js["id"].asInt();
    int groupid = js["groupid"].asInt();

    vector<int> useridVec = m_groupModel.queryGroupUsers(userid,groupid);
    for(int id : useridVec)
    {
        lock_guard<mutex> lock(m_connMutex);

        Json::StreamWriterBuilder writer;
        string respStr = Json::writeString(writer,js);

        auto it = m_userConnMap.find(id);
        if(it != m_userConnMap.end())
        {
            it->second->send(respStr);
        }
        else
        {
            User user = m_userModel.query(id);
            if(user.getState() == "online")
            {
                m_redis.publish(id,respStr);
            }
            else
            {
                m_offlineMsgModel.insert(id,respStr);
            }
            
        }
    }


}

void chatservice::chat1by1(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int toid = js["toid"].asInt();

    Json::StreamWriterBuilder writer;
    string respStr = Json::writeString(writer,js);
    if(respStr.empty())
    {
        LOG_ERROR<<__FILE__<<__LINE__<<":"<<"JSON parse ERROR";
        return;
    }

    {
        lock_guard<mutex> lock(m_connMutex);
        auto it = m_userConnMap.find(toid);

        if(it != m_userConnMap.end())
        {   
            it->second->send(respStr.c_str());
            return ;
        }
    }

    User user = m_userModel.query(toid);
    if(user.getState() == "online")
    {
        m_redis.publish(toid,respStr);
        return;
    }


    m_offlineMsgModel.insert(toid,respStr);

    
}

void chatservice::loginout(const TcpConnectionPtr &conn, Json::Value &js, Timestamp time)
{
    int userid = js["id"].asInt();
    {
        lock_guard<mutex> lock(m_connMutex);
        
        auto it = m_userConnMap.find(userid);
        if(it->second == conn)
        {
            m_userConnMap.erase(it);
        }
        else
        {
            return;
        }
        
    }

    m_redis.unsubscribe(userid);
    User user(userid,"","","offline");
    user.setState("offline");
    m_userModel.updateState(user);


}

msghandler chatservice::getHandler(int msgid)
{
    auto it = m_msghandlerMap.find(msgid);
    if(it == m_msghandlerMap.end())
    {
        
        return [=](const TcpConnectionPtr&conn,Json::Value &js,Timestamp time){
            LOG_ERROR<<__FILE__<<":"<<__LINE__<<"msgid : "<<msgid<<"can not find handler!";
        };
    }
    return m_msghandlerMap[msgid];
}

void chatservice::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {    
        lock_guard<mutex> lock(m_connMutex);
    
        for(auto it = m_userConnMap.begin();it!=m_userConnMap.end();it++)
        {
            if(it->second == conn)
            {
                user.setId(it->first);
                m_userConnMap.erase(it);
                break;
            }
        }
    }

    m_redis.unsubscribe(user.getId());

    if(user.getId() != -1)
    {
        user.setState("offline");
        m_userModel.updateState(user);
    }
    

}

void chatservice::handleRedisSubcribeMessage(int userid,string msg)
{
    Json::Reader reader;
    Json::Value js;

    reader.parse(msg,js);

    lock_guard<mutex> lock(m_connMutex);
    auto it = m_userConnMap.find(userid);
    if(it != m_userConnMap.end())
    {
        it->second->send(msg);
        return;
    }
    m_offlineMsgModel.insert(userid,msg);

}

