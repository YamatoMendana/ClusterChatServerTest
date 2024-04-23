#include "chatserver.h"
#include "chatservice.h"
#include <functional>
#include <jsoncpp/json/json.h>


using namespace std;
using namespace placeholders;


chatserver::chatserver(EventLoop *loop, const InetAddress &listenaddr, const string &namearg)
:m_server(loop,listenaddr,namearg),m_loop(loop)
{
    m_server.setConnectionCallback(std::bind(&chatserver::onConnection,this,_1));

    m_server.setMessageCallback(std::bind(&chatserver::onMessage,this,_1,_2,_3)); 

    m_server.setThreadNum(4);

}

void chatserver::start()
{
    m_server.start();
}

void chatserver::onConnection(const TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        chatservice::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void chatserver::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    
    Json::Value root;
    Json::Reader reader;
    bool bret = reader.parse(buf,root);
    auto handler = chatservice::instance()->getHandler(root["msgid"].asInt());
    handler(conn,root,time);

}
