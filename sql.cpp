#include "sql.h"
#include <QDebug>

SQL::SQL()
{

}

void SQL::opensql(QString sqlname)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sqlname);
    if(!db.open())
    {
        qDebug()<<"open error";
    }
}

void SQL::creattable(QString tablename)
{
    //创建表格
    QString sql = QString("create table if not exists %1(id integer PRIMARY KEY autoincrement, songid text, songmsg text, picurl text, songurl text)").arg(tablename);
    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug()<<"create error";
    }
}

void SQL::inserttable(QString tablename,QString songid, QString songmsg, QString picurl, QString songurl)
{

    //插入数据
    QSqlQuery query;
    QString sql = QString("insert into %1(songid, songmsg, picurl, songurl) values('%2','%3','%4','%5')").arg(tablename).arg(songid).arg(songmsg).arg(picurl).arg(songurl);
    if(!query.exec(sql))
    {
        qDebug()<<"insert error";
    }

}



void SQL::selecttable()
{
    //查询数据
    QSqlQuery query;
    query.exec("select * from searchmusiclist");
    if(!query.exec())
    {
        qDebug()<<"select error";
    }
    else
    {
        while(query.next())
        {
            QString id = query.value(1).toString();
            QString songmsg = query.value(2).toString();
            QString picurl = query.value(3).toString();
            QString songurl = query.value(4).toString();
            qDebug()<<QString("songid:%1    songmsg:%2    picurl:%3    songurl:%4").arg(id).arg(songmsg).arg(picurl).arg(songurl);
        }
    }
}



void SQL::deletetable(QString tablename)
{
    //清空表格
    QString sql = QString("DROP TABLE %1").arg(tablename);
    QSqlQuery query;
    if(!query.exec(sql))
    {
        qDebug()<<"delete error";
    }
    creattable(tablename);
}

