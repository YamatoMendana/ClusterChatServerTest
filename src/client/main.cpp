#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>

#include "unistd.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unordered_map>
#include <functional>

#include <jsoncpp/json/json.h>

#include "group.h"
#include "user.h"
#include "../public/public.h"

using namespace std;



User g_currentUser;
vector<User> g_currentUserFriendVec;
vector<Group> g_currentUserGroupVec;
bool isMainMenuRuning = false;

void showCurrentUserData();

void readTaskHandler(int clientfd);

string getCurrentTime();

void help(int fd = 0,string str = "");

void chat(int,string);

void addfriend(int,string);

void creategroup(int,string);

void addgroup(int,string);

void groupchat(int,string);

void loginout(int,string);

void mainMenu(int clientfd);


void showCurrentUserData()
{
    cout<<"==========================================login User============================================"<<endl;
    cout<<"current login user => id:"<<g_currentUser.getId()<< "  name:"<<g_currentUser.getName()<<endl;
    cout<<"------------------------------friend list-----------------------------"<<endl;
    if(!g_currentUserFriendVec.empty())
    {
        for(User& user : g_currentUserFriendVec)
        {
            cout<< user.getId()<<"  "<<user.getName()<<"  "<<user.getState()<<endl;

        }
    }

    cout<<"------------------------------group list-----------------------------"<<endl;
    if(!g_currentUserGroupVec.empty())
    {
        for(Group& group : g_currentUserGroupVec)
        {
            cout<< group.getId()<<"  "<<group.getName()<<"  "<<group.getDesc()<<endl;
            for(GroupUser& user:group.getUsers())
            {
                cout<<user.getId()<<"  "<< user.getName()<<"  "<<user.getState()<<user.getRole()<<endl;
            }

        }
    }

    cout<<"========================================= end ====================================================="<<endl;
}

void readTaskHandler(int clientfd)
{
    while(1)
    {
        char buffer[1024] = {0};
        int len = recv(clientfd,buffer,sizeof(buffer),0);
        if(len == -1 || len == 0)
        {
            close(clientfd);
            exit(-1);
        }

        Json::Value js;
        Json::Reader reader;
        reader.parse(buffer,js);
        int msgType = js["msgid"].asInt();
        if(msgType == CHAT1BY1_MSG)
        {
            cout<<js["time"].asString()<<"[" <<js["id"].asInt()<<"]"<<js["name"].asString()
            <<" said: "<<js["msg"].asString()<<endl;
            continue;
        }
        else if(msgType == GROUP_CHAT_MSG)
        {
            cout<<"群消息:"<<"["<<js["groupid"].asInt()<<"]"<< js["time"].asString()<<"[" <<js["id"].asInt()<<"]"<<js["name"].asString()
            <<" said: "<<js["msg"].asString()<<endl;
            continue;
        }

    }

}

string getCurrentTime()
{
    auto tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char strtime[64] ={0};
    sprintf(strtime,"%d-%02d-%02d %02d:%02d:%02d",
        (int)ptm->tm_year+1900,(int)ptm->tm_mon+1,(int)ptm->tm_mday,
        (int)ptm->tm_hour,(int)ptm->tm_min,(int)ptm->tm_sec);

    return string(strtime);

}



unordered_map<string,string> commmandMap = {
    {"help","显示所有支持的命令,格式help"},
    {"chat","一对一聊天,格式chat:friendid:message"},
    {"addfriend","添加好友,格式addfriend:friendid"},
    {"creategroup","添加群组,格式creategroup:groupname:groupdesc"},
    {"addgroup","加入群组,格式addgroup:groupid"},
    {"groupchat","群聊,格式groupchat:groupid:message"},
    {"loginout","注销,格式loginout"}
};

unordered_map<string, std::function<void(int,string)>> commandHandlerMap = {
    {"help",help},
    {"chat",chat},
    {"addfriend",addfriend},
    {"creategroup",creategroup},
    {"addgroup",addgroup},
    {"groupchat",groupchat},
    {"loginout",loginout}
};

void mainMenu(int clientfd)
{
    help();

    char buffer[1024] = {0};
    while(isMainMenuRuning)
    {
        cin.getline(buffer,1024);
        string commmandbuf(buffer);
        string command;
        int idx = commmandbuf.find(":");
        if(idx == -1)
        {
            command = commmandbuf;
        }
        else
        {
            command = commmandbuf.substr(0,idx);
        }

        auto it = commandHandlerMap.find(command);
        if(it == commandHandlerMap.end())
        {
            ChatLog::instance()->CHAT_LOG_ERROR("invalid input command!");
            continue;
        }

        it->second(clientfd,commmandbuf.substr(idx + 1,commmandbuf.size() - idx));
    }
    
}

void help(int fd ,string str)
{
    cout<<"show command list" << endl;
    for(auto& p : commmandMap)
    {
        cout<<p.first << " : "<< p.second<<endl;

    }
    cout<<endl;
}

void chat(int clientfd,string str)
{
    int idx = str.find(":");
    if(idx == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("chat command invalid!");
        return ;
    }
    string strfriendid = str.substr(0,idx);
    int friendid = atoi(strfriendid.c_str());
    string message = str.substr(idx + 1,str.size() - idx);

    Json::Value js;
    js["msgid"] = CHAT1BY1_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = getCurrentTime();

    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js);
     
    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send chat msg error");
    }
}


void addfriend(int clientfd,string str)
{
    int friendid = atoi(str.c_str());
    Json::Value js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js);
     
    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send addfriend msg error");
    }
}

void creategroup(int clientfd,string str)
{
    int idx = str.find(":");
    if(idx == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("creategroup command invalid!");
        return;
    }

    string groupname = str.substr(0,idx).c_str();
    string groupdesc = str.substr(idx + 1,str.size() -  idx);

    Json::Value js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js); 

    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send creategroup msg error");
    }

}

void addgroup(int clientfd,string str)
{
    int groupid = atoi(str.c_str());

    Json::Value js;
    js["msgid"] = ADD_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupid"] = groupid;
    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js); 

    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send addgroup msg error");
    }
}

void groupchat(int clientfd,string str)
{
    int idx = str.find(":");
    if(idx == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("groupchat command invalid!");
        return;
    }

    int groupid = atoi(str.substr(0,idx).c_str());
    string message = str.substr(idx + 1,str.size() -  idx);

    Json::Value js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["groupid"] = groupid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js); 

    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send groupchat msg error ->" + str);

    }

}

void loginout(int clientfd,string str)
{
    Json::Value js;
    js["msgid"] = LOGIN_OUT_MSG;
    js["id"] = g_currentUser.getId();

    Json::StreamWriterBuilder writer;
    string reqStr = Json::writeString(writer,js); 

    int len = send(clientfd,reqStr.c_str(),strlen(reqStr.c_str()) + 1,0);
    if(len == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("send addgroup msg error");
    }
    else
    {
        isMainMenuRuning = false;
    }
}

int main(int argc,char* argv[])
{
    if(argc < 3)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("command invalid! example : ./chatclient 127.0.0.1 6000");
        exit(-1);
    }

    char*ip = argv[1];
    uint16_t port = atoi(argv[2]);

    int clientfd = socket(AF_INET,SOCK_STREAM ,0);
    if(clientfd == -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("socket create error");
        exit(-1);
    }

    sockaddr_in server;
    memset(&server,0,sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientfd,(sockaddr*)&server,sizeof(sockaddr_in))== -1)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("connect server error");
        close(clientfd);
        exit(-1);
    }

    while(1)
    {
        cout<<"=============================="<<endl;
        cout<<"1.login"<<endl;
        cout<<"2.register"<<endl;
        cout<<"3.quit"<<endl;
        cout<<"=============================="<<endl;
        cout<<"choice:"<<endl;
        int choice = 0;
        cin>>choice;
        cin.get();
        int len;

        switch(choice)
        {
        //登录
        case 1:
        {
            int id = 0;
            char pwd[64] = {0};
            cout<<"userid:";
            cin>>id;
            cin.get();
            cout<<"userpassword:";
            cin.getline(pwd,sizeof(pwd));

            Json::Value js;
            js["msgid"] = LOGIN_MSG;
            js["id"] = id;
            js["password"] = pwd;

            Json::StreamWriterBuilder writer;

            string strjs = Json::writeString(writer,js);
            len = send(clientfd, strjs.c_str(), strlen(strjs.c_str()) + 1,0);
            if(len == -1)
            {
                ChatLog::instance()->CHAT_LOG_ERROR("send login msg error");
            }
            else
            {
                char recvBuf[1024] = {0};
                len = recv(clientfd,recvBuf,sizeof(recvBuf),0);
                if(len == -1)
                {
                    ChatLog::instance()->CHAT_LOG_ERROR("recv login msg error");
                }
                
                ChatLog::instance()->CHAT_LOG_INFO(recvBuf);

                Json::Reader reader;
                Json::Value respJs;
                Json::Value userJs;
                Json::Value offlineMsgArrJs;
                Json::Value offlineMsgJs;

                reader.parse(recvBuf,respJs);
                if(respJs["errno"].asInt())
                {
                    ChatLog::instance()->CHAT_LOG_ERROR(respJs["errorMsg"].asString());
                }
                else
                {
                    g_currentUser.setId(respJs["id"].asInt());
                    g_currentUser.setName(respJs["name"].asString());


                    if(respJs.isObject())
                    {
                        g_currentUserFriendVec.clear();


                        Json::Value friendJs;
                        Json::Value groupJs;
                        if(respJs.isMember("friends") && respJs["friends"].isArray())
                        {
                            friendJs = respJs["friends"];
                            int friendsSize = friendJs.size();
                            for(int j = 0;j<friendsSize;j++)
                            {
                                User user;
                                user.setId(friendJs[j]["id"].asInt());
                                user.setName(friendJs[j]["name"].asString());
                                user.setState(friendJs[j]["state"].asString());
                                g_currentUserFriendVec.push_back(user);
                            }
                        }
                        else
                        {
                            ChatLog::instance()->CHAT_LOG_ERROR("friendJs parse fail");
                            // close(clientfd);
                            // exit(0);
                        }
                            

                        if(respJs.isMember("groups") && respJs["groups"].isArray())
                        {
                            g_currentUserGroupVec.clear();

                            groupJs = respJs["groups"];
                            int groupsize = groupJs.size();
                            for(int j = 0; j < groupsize;j++)
                            {
                                Group group;
                                group.setId(groupJs[j]["id"].asInt());
                                group.setName(groupJs[j]["groupname"].asString());
                                group.setDesc(groupJs[j]["groupdesc"].asString());


                                if(groupJs[j].isMember("users") && groupJs[j]["users"].isArray())
                                {
                                    userJs = groupJs[j]["users"];
                                    int useSize = userJs.size();
                                    for(int k =0 ;k<useSize;k++)
                                    {
                                        GroupUser guser;
                                        guser.setId(userJs[k]["id"].asInt());
                                        guser.setName(userJs[k]["name"].asString());
                                        guser.setState(userJs[k]["state"].asString());
                                        guser.setRole(userJs[k]["role"].asString());
                                        group.getUsers().push_back(guser);
                                    }

                                    g_currentUserGroupVec.push_back(group);
                                }
                                else
                                {
                                    ChatLog::instance()->CHAT_LOG_ERROR("userJs parse fail");
                                    close(clientfd);
                                    exit(0);
                                }

                                
                            }

                        }
                        else
                        {
                            ChatLog::instance()->CHAT_LOG_ERROR("groupJs parse fail");
                            // close(clientfd);
                            // exit(0);
                        }
                        
                    }
                    showCurrentUserData();

                    if(respJs.isMember("offlineMsg") && respJs["offlineMsg"].isArray())
                    {
                        offlineMsgArrJs = respJs["offlineMsg"];
                        int offlineMsgArrSize = offlineMsgArrJs.size();
                        for(int j = 0;j < offlineMsgArrSize;j++)
                        {
                            Json::Reader reader;
                            reader.parse(offlineMsgArrJs[j].asString(),offlineMsgJs);

                            int msgType = offlineMsgJs["msgid"].asInt();
                            if(msgType == CHAT1BY1_MSG)
                            {
                                cout<<offlineMsgJs["time"].asString()<<"[" <<offlineMsgJs["id"].asInt()<<"]"<<offlineMsgJs["name"].asString()
                                <<" said: "<<offlineMsgJs["msg"].asString()<<endl;
                                continue;
                            }
                            else if(msgType == GROUP_CHAT_MSG)
                            {
                                cout<<"群消息:"<<"["<<offlineMsgJs["groupid"].asInt()<<"]"<< offlineMsgJs["time"].asString()
                                <<"[" <<offlineMsgJs["id"].asInt()<<"]"<<offlineMsgJs["name"].asString()
                                <<" said: "<<offlineMsgJs["msg"].asString()<<endl;
                                continue;
                            }
                        }
                        
                    }
                    else
                    {
                        ChatLog::instance()->CHAT_LOG_ERROR("offlineMsgJs parse fail");
                        // close(clientfd);
                        // exit(0);
                    }

                    static int threadnum=0;
                    if(threadnum == 0)
                    {
                        thread readTask(readTaskHandler,clientfd);
                        readTask.detach();
                        threadnum++;
                    }

                    
                    
                    isMainMenuRuning=true; 
                    mainMenu(clientfd);

                    
                }

            }
            break; 
        }
        //注册
        case 2:
        {
            char name[64] = {0};
            char pwd[64] = {0};
            cout<<"username:";
            cin.getline(name,50);
            cout<<"userpassword:";
            cin.getline(pwd,50);

            Json::Value js;
            js["msgid"] = REGISTER_MSG;
            js["name"] = name;
            js["password"] = pwd;

            Json::StreamWriterBuilder writer;
            string strjs = Json::writeString(writer,js);

            int len = send(clientfd, strjs.c_str(), strlen(strjs.c_str()) + 1,0);
            if(len == -1)
            {
                ChatLog::instance()->CHAT_LOG_ERROR("send reg response error");
            }
            else
            {
                Json::Reader reader;
                Json::Value respJs;
                reader.parse(strjs,respJs);
                if(respJs["errno"].asInt())
                {
                    ChatLog::instance()->CHAT_LOG_ERROR(respJs["errorMsg"].asString());
                }
                else
                {
                    cout<< name<<" register success userid is "<<respJs["id"].asInt()<<endl;

                }
            }
            break;
        }
        case 3:
        {    
            close(clientfd);
            exit(0);
        }
        default:
            cerr<<"invalid input"<<endl;
            break;
        }
    }


    return 0;
}

