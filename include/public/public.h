#ifndef PUBLIC_H
#define PUBLIC_H

#include <stdio.h>
#include <iostream>


using namespace std;

enum emMsgType
{
    LOGIN_MSG =1 ,//登录信息
    LOGIN_MSG_ACK,//登录响应信息
    LOGIN_OUT_MSG, //登出注销
    REGISTER_MSG,//注册信息
    REGISTER_MSG_ACK,//注册响应信息

    CHAT1BY1_MSG, // 一对一聊天消息

    ADD_FRIEND_MSG, //添加好友信息

    CREATE_GROUP_MSG,//创建群组
    ADD_GROUP_MSG, //添加群组
    GROUP_CHAT_MSG,//群组聊天



};



class ChatLog
{
public:
    //获取单例对象接口
    static ChatLog* instance()
    {
        static ChatLog service;
        return &service;
    }

    void CHAT_LOG_INFO(string err)
    {
        cerr<<"[INFO]"<<__FILE__<<":"<<__LINE__<<":"<< err <<endl;
    }


    void CHAT_LOG_ERROR(string err)
    {
        cerr<<"[ERROR]" <<__FILE__<<":"<<__LINE__<<":"<< err <<endl;
    }
private:
    ChatLog(){};
};



#endif