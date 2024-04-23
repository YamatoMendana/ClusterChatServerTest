#ifndef CHATSERVICE_H
#define CHATSERVICE_H


#include <unordered_map>
#include <functional>
#include <muduo/net/TcpConnection.h>
#include <jsoncpp/json/json.h>
#include <mutex>

#include "model/userModel.h"
#include "model/offlineMsgModel.h"
#include "model/friendmodel.h"
#include "model/groupModel.h"
#include "redis/credis.h"

using namespace std;
using namespace muduo;
using namespace muduo::net;

//表示处理消息的事件回调方法类型
using msghandler = std::function<void(const TcpConnectionPtr& conn,Json::Value &root,Timestamp time)>;

class chatservice
{
public:
    //获取单例对象接口
    static chatservice* instance();
    //处理登录业务
    void login(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr& conn,Json::Value js,Timestamp time);
    //添加好友业务
    void addFriend(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //创建群组业务
    void createGroup(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //加入群组业务
    void addGroup(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //群组聊天业务
    void groupChat(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //处理注销业务
    void loginout(const TcpConnectionPtr& conn,Json::Value& js,Timestamp time);
    //获取消息对应处理器
    msghandler getHandler(int msgid);
    //处理客户端异常操作
    void clientCloseException(const TcpConnectionPtr& conn);
    //一对一聊天
    void chat1by1(const TcpConnectionPtr& conn, Json::Value& js,Timestamp time);
    //服务器异常，业务重置方法
    void reset();

    void handleRedisSubcribeMessage(int userid,string msg);

private:
    chatservice();

    unordered_map<int,msghandler> m_msghandlerMap;

    unordered_map<int,TcpConnectionPtr> m_userConnMap;

    mutex m_connMutex;

    UserModel m_userModel;

    OfflineMsgModel m_offlineMsgModel;
    
    FriendModel m_friendModel;

    GroupModel m_groupModel;

    cRedis m_redis;

};






#endif