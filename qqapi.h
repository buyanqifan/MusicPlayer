#ifndef QQAPI_H
#define QQAPI_H

#include "sql.h"
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class qqapi : public QObject
{
    Q_OBJECT
public:
    qqapi();

    void search(QString msg);
    void nextpage(QString msg);
    void prepage(QString msg);

signals:
    void Finishsignal();

private slots:
    void read_data(QNetworkReply *reply);
    void sendfisish(int id);

private:
    //管理器
    QNetworkAccessManager manager;
    QNetworkAccessManager manager_url;

    //数据库
    SQL *sql;

    int sum = 0;

    //音乐搜索 API
    //参数%1：搜索关键字
    //参数%2：页数
    const QString ApiOfSearch = "https://c.y.qq.com/soso/fcgi-bin/client_search_cp?w=%1&p=%2&n=20&format=json";
    qint32 searchoffset;    //搜索页数  默认为1
    //歌曲purl API
    //参数%1：songmid
    const QString ApiOfSongPurl = "https://u.y.qq.com/cgi-bin/musicu.fcg?format=json&data=%7B%22req_0%22%3A%7B%22module%22%3A%22vkey.GetVkeyServer%22%2C%22method%22%3A%22CgiGetVkey%22%2C%22param%22%3A%7B%22guid%22%3A%22358840384%22%2C%22songmid%22%3A%5B%22%1%22%5D%2C%22songtype%22%3A%5B0%5D%2C%22uin%22%3A%221443481947%22%2C%22loginflag%22%3A1%2C%22platform%22%3A%2220%22%7D%7D%2C%22comm%22%3A%7B%22uin%22%3A%2218585073516%22%2C%22format%22%3A%22json%22%2C%22ct%22%3A24%2C%22cv%22%3A0%7D%7D";
    //专辑封面 API
    //参数%1：albumMid
    const QString ApiOfPicurl = "http://y.gtimg.cn/music/photo_new/T002R180x180M000%1.jpg";
    //歌曲url API
    //参数%1：purl
    const QString ApiOfSongurl = "http://ws.stream.qqmusic.qq.com/%1";
};


class getqqurl : public QObject
{
    Q_OBJECT
public:
    getqqurl();

    void getpurl(QString picurl, int i);
    QString getsongpurl();
private slots:
    void read_data(QNetworkReply *reply);


signals:
    void purlsignal(int id);
private:
    QNetworkAccessManager manager;

    QString songpurl;
    int id;
    //歌曲purl API
    //参数%1：songmid
    const QString ApiOfSongPurl = "https://u.y.qq.com/cgi-bin/musicu.fcg?format=json&data=%7B%22req_0%22%3A%7B%22module%22%3A%22vkey.GetVkeyServer%22%2C%22method%22%3A%22CgiGetVkey%22%2C%22param%22%3A%7B%22guid%22%3A%22358840384%22%2C%22songmid%22%3A%5B%22%1%22%5D%2C%22songtype%22%3A%5B0%5D%2C%22uin%22%3A%221443481947%22%2C%22loginflag%22%3A1%2C%22platform%22%3A%2220%22%7D%7D%2C%22comm%22%3A%7B%22uin%22%3A%2218585073516%22%2C%22format%22%3A%22json%22%2C%22ct%22%3A24%2C%22cv%22%3A0%7D%7D";
    //歌曲url API
    //参数%1：purl
    const QString ApiOfSongurl = "http://ws.stream.qqmusic.qq.com/%1";

};




#endif // QQAPI_H
