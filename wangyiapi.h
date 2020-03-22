#ifndef WANGYIAPI_H
#define WANGYIAPI_H

#include "sql.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Wangyiapi : public QObject
{
    Q_OBJECT
public:
    Wangyiapi();

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

    //数据库
    SQL *sql;

    //音乐搜索API
    //参数%1：搜索关键字
    //参数%2：页数
    const QString ApiOfSearch = "https://api.imjad.cn/cloudmusic/?type=search&s=%1&offset=%2";
    qint32 searchoffset;    //搜索页数
    //歌曲链接API
    //参数%1：歌曲id
    const QString ApiOfSongLink = "https://music.163.com/song/media/outer/url?id=%1.mp3";

};

#endif // WANGYIAPI_H
