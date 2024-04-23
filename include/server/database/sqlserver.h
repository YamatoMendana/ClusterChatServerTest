#ifndef SQLSERVER_H
#define SQLSERVER_H

#include <iostream>
#include <string>
#include <mysql/mysql.h>
#include <muduo/base/Logging.h>
#include <jsoncpp/json/json.h>

using namespace std;

static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat";

class SQL_Base
{
public:
    SQL_Base(){};
    ~SQL_Base(){};
    virtual bool Connect() = 0;

    virtual bool Insert(string strsql) = 0;
    virtual bool Del(string strsql) = 0;

    virtual bool Update(string strsql) = 0;
    virtual string Query(string strsql) = 0;
    virtual bool Commit() = 0;

    virtual uint64_t get_insert_id() = 0;

};


class cMySQL : public SQL_Base
{
public:
    cMySQL();
    ~cMySQL();

    virtual bool Connect();

    virtual bool Insert(string strsql);

    virtual bool Del(string strsql);

    virtual bool Update(string strsql);

    virtual string Query(string strsql);

    virtual bool Commit();

    virtual uint64_t get_insert_id(); 

    MYSQL* getConnection();

    int useDatabase(string dbname);
private:
    

    MYSQL* m_conn;

};




#endif