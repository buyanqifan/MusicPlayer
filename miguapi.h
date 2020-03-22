#ifndef MIGUAPI_H
#define MIGUAPI_H

#include "sql.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Miguapi : public QObject
{
    Q_OBJECT
public:
    Miguapi();

    void search(QString msg);
    void nextpage(QString msg);
    void prepage(QString msg);

signals:
    void Finishsignal();

private slots:
    void read_data(QNetworkReply *reply);

private:
    //管理器
    QNetworkAccessManager manager;
    QNetworkAccessManager managerpic;

    //数据库
    SQL *sql;

    //音乐搜索API
    //参数%1：搜索关键字
    //参数%2：页数
    const QString ApiOfSearch = "https://v1.itooi.cn/migu/search?keyword=%1&type=song&pageSize=10&page=%2";
    qint32 searchoffset;    //搜索页数
    //歌曲链接API
    //参数%1：歌曲id
    const QString ApiOfSongLink = "https://v1.itooi.cn/migu/url?id=%1&quality=320";
    //图片API
    //参数%1：歌曲id
    const QString ApiOfPicurl = "https://v1.itooi.cn/migu/pic?id=%1&isRedirect=0";
};

#endif // MIGUAPI_H
