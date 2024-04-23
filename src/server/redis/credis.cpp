#include "credis.h"
#include <iostream>
using namespace std;

cRedis::cRedis()
    : m_publicCtx(nullptr),m_subcribeCtx(nullptr)
{

}

cRedis::~cRedis()
{
    if(m_publicCtx != nullptr)
    {
        redisFree(m_publicCtx);
    }

    if(m_subcribeCtx != nullptr)
    {
        redisFree(m_subcribeCtx);
    }
}

bool cRedis::connect()
{
    m_publicCtx = redisConnect("127.0.0.1",6379);
    if(m_publicCtx == nullptr)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("connect redis failed!");
        return false;
    }

    m_subcribeCtx = redisConnect("127.0.0.1",6379);
    if(m_subcribeCtx == nullptr)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("connect redis failed!");
        return false;
    }

    thread t( [&](){
        observer_channel_message();
    });
    t.detach();

    ChatLog::instance()->CHAT_LOG_INFO("connect redis-server success");

    return true;
}

bool cRedis::publish(int channel, string message)
{
    redisReply* reply = (redisReply*)redisCommand(m_publicCtx,"PUBLISH %d %s",channel,message.c_str());
    if(reply == nullptr)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("publish command failed!");
        return false;
    }

    freeReplyObject(reply);

    return true;
}

bool cRedis::subscribe(int channel)
{

    if(REDIS_ERR == redisAppendCommand(this->m_subcribeCtx,"subscribe %d",channel)) 
    {
        ChatLog::instance()->CHAT_LOG_ERROR("subscribe command failed!");
        return false;
    }

    int done = 0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->m_subcribeCtx,&done))
        {
            ChatLog::instance()->CHAT_LOG_ERROR("subscribe command failed!");
            return false;
        }
    }

    return true;
}

bool cRedis::unsubscribe(int channel)
{
    if(REDIS_ERR == redisAppendCommand(this->m_subcribeCtx,"unsubscribe %d",channel)) 
    {
        ChatLog::instance()->CHAT_LOG_ERROR("unsubscribe command failed!");
        return false;
    }

    int done = 0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->m_subcribeCtx,&done))
        {
            ChatLog::instance()->CHAT_LOG_ERROR("unsubscribe command failed!");
            return false;
        }
    }

    return true;
}

//在独立线程中接收订阅通道信息
void cRedis::observer_channel_message()
{
    redisReply* reply = nullptr;
    while(REDIS_OK == redisGetReply(this->m_subcribeCtx,(void**)&reply))
    {
        //订阅收到的信息是一个带三元素的数组
        if( reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            //业务层上报通道上的信息
            m_notify_message_handler(atoi(reply->element[1]->str), reply->element[2]->str);

        }
        freeReplyObject(reply);

    }

}

void cRedis::init_notify_handler(function<void(int, string)> fn)
{
    this->m_notify_message_handler = fn;

}
