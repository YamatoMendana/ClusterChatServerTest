# ClusterChatServerTest
基于muduo网络库的聊天服务器,用于nginx 负载均衡+ redis订阅发布 的服务器与客户端代码练习

编译方式
在build文件夹内执行（如没有该文件夹，请自行创建）
cmake .. 
生成makefile相关文件会在build内
make


需要的环境：
redis-7.2.4
mysql 5.7及以上

ldd 相关库文件：
boost库
muduo库
hiredis 库
mysqlclient 库
jsoncpp 库
