#ifndef KUGOUAPI_H
#define KUGOUAPI_H

#include "sql.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Kugouapi : public QObject
{
    Q_OBJECT
public:
    Kugouapi();

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
    const QString ApiOfSearch = "https://v1.itooi.cn/kugou/search?keyword=%1&type=song&pageSize=20&page=%2";
    qint32 searchoffset;    //搜索页数
    //歌曲链接API
    //参数%1：歌曲id
    const QString ApiOfSongLink = "https://v1.itooi.cn/kugou/url?id=%1&quality=flac";
    //图片API
    //参数%1：歌曲id
    const QString ApiOfPicurl = "https://v1.itooi.cn/kugou/pic?id=%1&isRedirect=0";
};


#endif // KUGOUAPI_H
