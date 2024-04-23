#include <iostream>
#include <stdio.h>
#include <signal.h>
#include "chatserver.h"
#include "chatservice.h"

#include "../public/public.h"

using namespace std;

void resetHandle(int)
{
    chatservice::instance()->reset();
    exit(0);
}

int main(int argc,char* argv[])
{   

    if(argc < 3)
    {
        ChatLog::instance()->CHAT_LOG_ERROR("command invalid! example : ./chatserver 127.0.0.1 6000");
        exit(-1);
    }

    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT | SIGTERM,resetHandle);

    EventLoop loop;
    InetAddress addr(ip,port);
    chatserver server(&loop,addr,"chatserver");

    server.start();
    loop.loop();
    
    return 0;
}

// int main()
// {
//     signal(SIGINT | SIGTERM,resetHandle);

//     EventLoop loop;
//     InetAddress addr("127.0.0.1",6000);
//     chatserver server(&loop,addr,"chatserver");

//     server.start();
//     loop.loop();
    
//     return 0;

// }