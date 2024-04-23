#ifndef CREDIS_H
#define CREDIS_H


#include <hiredis/hiredis.h>
#include <thread>
#include <functional>

#include "../public/public.h"

using namespace std;

class cRedis
{
public:
    cRedis();
    ~cRedis();

    //连接redis服务器
    bool connect();

    //向redis指定通道channel发布消息
    bool publish(int channel,string message);

    //向redis指定通道subcribe订阅信息
    bool subscribe(int channel);

    //向redis指定通道unsubcribe 取消订阅信息
    bool unsubscribe(int channel);

    //在独立线程中接收订阅通道的信息
    void observer_channel_message();

    //向初始化业务层上报通道消息的回调对象
    void init_notify_handler(function<void(int,string)> fn);

private:
    redisContext* m_publicCtx;

    redisContext* m_subcribeCtx;

    //回调操作，收到订阅信息，给service层上报
    function<void(int,string)> m_notify_message_handler;


};



#endif