#include <mysql++.h>
#include <ssqls.h> //sql_create_#
#include <stdlib.h>
#include <fstream>
#include <string>
// #pragma pack(1)
/*
Query 使用：
1）execute
用于不返回数据的查询，该函数返回一个SimpleResult对象。

2）exec
它返回一个bool值，标示执行成功与否

3）store
用于用服务器获取数据，该函数返回一个StoreQueryResult对象。可以使用[index]和[field]取数据。

4）use
用于用服务器获取数据，该函数返回UseQueryResult对象。
相比store()而言更节省内存，该对象类似StoreQueryResult,但是不提供随机访问的特性。
use查询会让服务器一次返回结果集的一行。
Query对象的errnum()返回上次执行对应的错误代码，error()返回错误信息，affected_rows()返回受影响的行数。
*/

//Table of the same name: user
sql_create_6(user, 1, 6,
             mysqlpp::sql_int, id,
             mysqlpp::sql_char, name,
             mysqlpp::sql_int, age,
             mysqlpp::sql_char, nick,
             mysqlpp::sql_date, createDateTime,
             mysqlpp::Null<mysqlpp::sql_blob>, data);

sql_create_2(images, 1, 2,
             mysqlpp::Null<mysqlpp::sql_int>, id,
             mysqlpp::Null<mysqlpp::sql_blob>, data);

int test_list_use(mysqlpp::Query &query)
{
    const char *sql = "select * from city limit 6";
    query << sql;
    std::cout << "test_list_use sql = " << query << std::endl;
    if (mysqlpp::UseQueryResult res = query.use())
    {
        std::cout << "We have:" << std::endl;
        while (mysqlpp::Row row = res.fetch_row())
        {
            std::cout << std::setw(5) << row["ID"] << ' ' << row["Name"] << ' ' << row["CountryCode"] << ' ' << row["District"] << ' ' << row["Population"] << std::endl;
            std::cout << "--->" << row[0] << ' ' << row[1] << ' ' << row[2] << ' ' << row[3] << ' ' << row[4] << std::endl;
        }
    }
    else
    {
        std::cerr << "query error: " << query.error() << std::endl;
        return -1;
    }
    return 0;
}

int test_list_store(mysqlpp::Query &query)
{
    const char *sql = "select * from city limit 6";
    query << sql;
    std::cout << "test_list_store sql = " << query << std::endl;
    if (mysqlpp::StoreQueryResult res = query.store())
    {
        std::cout << "We have:" << std::endl;
        for (auto it = res.cbegin(); it != res.cend(); ++it)
        {
            std::cout << std::left;
            mysqlpp::Row row = *it;
            for (int i = 0; i < row.size(); ++i)
                std::cout << std::setw(15) << row[i];
            std::cout << std::endl;
        }
    }
    else
    {
        std::cerr << "query error: " << query.error() << std::endl;
        return -1;
    }
    return 0;
}

int test_insert(mysqlpp::Query &query)
{
    time_t tm;
    time(&tm);
    std::cout << "--->1-" << mysqlpp::DateTime(tm) << std::endl;
    std::cout << "--->1-" << mysqlpp::DateTime(tm).year() << std::endl;
    user row(4, "a", 11, "c", mysqlpp::sql_date("1998-09-25"), mysqlpp::Null<mysqlpp::sql_blob>()); //Now()
    query.insert(row);
    if (auto res = query.exec())
    {
        std::cout << "insert success " << res << std::endl;
    }
    else
    {
        std::cout << "insert error " << query.error() << std::endl;
        return -1;
    }
    return 0;
}

int test_delete(mysqlpp::Connection &conn, mysqlpp::Query &query)
{
    const char *sql = "delete from user where id > 2";
    mysqlpp::Transaction trans(conn,
                               mysqlpp::Transaction::serializable,
                               mysqlpp::Transaction::session);
    query << sql;
    if (auto ret = query.exec())
    {
        std::cout << "delete success" << std::endl;
        // 提交事务
        trans.commit();
    }
    else
    {
        std::cout << "delete error: " << query.error() << std::endl;
    }
}

int test_put(mysqlpp::Query &query)
{
    query << "select * from user where name = " << mysqlpp::quote << "a";
    if (mysqlpp::StoreQueryResult res = query.store())
    {
        user row = res[0];
        user orig_row = row;
        row.name = "NB";
        query.update(orig_row, row);
        if (query.exec())
            std::cout << "update success" << std::endl;
        else
            std::cout << "update error:" << query.error() << std::endl;
    }
    else
    {
        std::cout << "no data" << std::endl;
    }
    return 0;
}

int test_binary(mysqlpp::Query &query)
{
    typedef struct stu
    {
        int age;
        char name[15];
        char nick[15];
    } stu;

    // if (!false)
    {
        images img;
        stu u = {12,"张三", "经2理"};
        int len = sizeof(u);
        char *pData = (char *)(&u);
        img.id = 1;
        img.data.data.assign(pData, len);
        query.insert(img);
        if (mysqlpp::SimpleResult res = query.execute())
        {
            std::cout << "Inserted  table, " << len << ' ' << img.data.data.size() << " bytes, ID " << res.insert_id() << std::endl;
        }
        else
        {
            std::cout << "insert error:" << query.error() << std::endl;
        }
        
    }
    // else
    {
        // Read from db
        char *sql = "select * from images";
        query << sql;
        std::vector<images> rows;
        query.storein(rows);
        for (auto it : rows)
        {
            stu *tmp = (stu*)(it.data.data.data());
            std::cout << " ==> " << tmp->age << " | " << tmp->name << " | "  << tmp->nick << std::endl;
        }
    }
    
}

int main()
{
    try
    {
        mysqlpp::Connection conn(false);
        conn.set_option(new mysqlpp::SetCharsetNameOption("utf8")); //中文乱码
        if (conn.connect("testDB", "10.5.2.236", "root", "ktgame"))
        {
            mysqlpp::Query query = conn.query();
            do
            {
                // test_list_use(query);
                // test_list_store(query);
                // if (test_insert(query) < 0)
                //     test_put(query);
                // test_delete(conn,query);
                test_binary(query);
            } while (false);
        }
        else
        {
            std::cout << "connect error:" << conn.error() << std::endl;
            return -1;
        }
    }
    catch (const mysqlpp::Exception &er)
    {
        // Catch-all for any other MySQL++ exceptions
        std::cerr << "Error: " << er.what() << std::endl;
        return -1;
    }

    return (EXIT_SUCCESS);
}
