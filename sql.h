#ifndef SQL_H
#define SQL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
class SQL : public QObject
{

public:
    SQL();

    void opensql(QString sqlname);

    void creattable(QString tablename);

    void inserttable(QString tablename,QString songid, QString songmsg, QString picurl, QString songurl);

    void selecttable();

    void deletetable(QString tablename);




};

#endif // SQL_H
