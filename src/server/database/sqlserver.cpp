#include "sqlserver.h"


cMySQL::cMySQL()
{
    m_conn = mysql_init(nullptr);

}

cMySQL::~cMySQL()
{
    if(m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
}

bool cMySQL::Connect()
{
    MYSQL *p = mysql_real_connect(m_conn,server.c_str(),user.c_str(),password.c_str(),dbname.c_str(),3306,nullptr,0);
    if(p !=nullptr)
    {
        // mysql_query(m_conn,"SET autocommit=0");
        mysql_query(m_conn,"set names gbk");
    }
    else
    {
        LOG_ERROR<<"connect mysql failed!";
    }

    return p;
}

bool cMySQL::Insert(string strsql)
{
    bool ret = Update(strsql);


    return ret;
}

bool cMySQL::Del(string strsql)
{
    bool ret = Update(strsql);


    return ret;
}

bool cMySQL::Update(string strsql)
{
    if(mysql_query(m_conn,strsql.c_str()))
    {
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<"更新失败";
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<"sql = "<<strsql;
        return false;
    }

    return true;
}

string cMySQL::Query(string strsql)
{
    string str = "";
    Json::Value resultArray;


    if(mysql_query(m_conn,strsql.c_str()))
    {
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<strsql<<"查询失败";
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<"sql = "<<strsql;
        return str;
    }

    MYSQL_RES* res = mysql_use_result(m_conn);
    if(res == nullptr)
    {
        LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<strsql<<"获取结果集失败";
        return str;
    }
    int row_num = mysql_num_fields(res);

    MYSQL_FIELD *fields = NULL;
    fields = mysql_fetch_fields(res);

    MYSQL_ROW row;
    while((row = mysql_fetch_row(res)) != NULL)
    {
        Json::Value resultObj;
        for(int i= 0;i<row_num;i++)
        {
            if(fields[i].type == MYSQL_TYPE_LONG)
                resultObj[fields[i].name]= atoi(row[i]);
            else if(fields[i].type == MYSQL_TYPE_VAR_STRING)
                resultObj[fields[i].name]= row[i];
            else
                resultObj[fields[i].name]= row[i];
            
        }

        resultArray.append(resultObj);
    }

    Json::StreamWriterBuilder writer;
    str = Json::writeString(writer,resultArray);

    // LOG_INFO<<"Query result = " << str;

    mysql_free_result(res);
    return str;
}

bool cMySQL::Commit()
{
    bool ret = mysql_commit(m_conn);

    return ret;
}

uint64_t cMySQL::get_insert_id()
{
    return mysql_insert_id(m_conn);
}

MYSQL *cMySQL::getConnection()
{
    return m_conn;
}

int cMySQL::useDatabase(string dbname)
{
    return mysql_select_db(m_conn,dbname.c_str());;
}
