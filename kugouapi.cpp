#include "kugouapi.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QEventLoop>

Kugouapi::Kugouapi()
{
    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    connect(&manager, &QNetworkAccessManager::finished, this, &Kugouapi::read_data);
}

void Kugouapi::search(QString msg)
{
    searchoffset = 0;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Kugouapi::nextpage(QString msg)
{
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Kugouapi::prepage(QString msg)
{
    if(searchoffset > 0) {
        searchoffset--;
    }
    else {
        return;
    }
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Kugouapi::read_data(QNetworkReply *reply)
{
    QString data = reply->readAll();
    //qDebug()<<data;
    //json解析
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(),&err);    //字符串数据与json对象转换
    if(err.error != QJsonParseError::NoError)   //检查data是否是json格式
    {
        qDebug()<<"json error";
        return;
    }

    //开始解析
    QJsonObject obj = doc.object();
    //根据data获取对象
    QJsonObject dataobj = obj.value("data").toObject();
    //根据info获取数组
    QJsonArray infoarr = dataobj.value("info").toArray();
    //遍历数组
    for(int i = 0; i<infoarr.count(); i++)
    {
        QJsonObject songobj = infoarr.at(i).toObject();    //获取数组中的第i个对象

        //根据filename获取songmsg
        QString songmsg = songobj.value("filename").toString();
        //根据hash获取songid
        QString songid = songobj.value("hash").toString();

        QUrl url(ApiOfPicurl.arg(songid));
        QNetworkRequest requestpic(url);
        //发送请求
        QNetworkReply* replypic = managerpic.get(requestpic);

        //等待读取数据
        QEventLoop eventLoop;
        connect(replypic, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);

        QString data = replypic->readAll();
        //qDebug()<<data;
        //json解析
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(),&err);    //字符串数据与json对象转换
        if(err.error != QJsonParseError::NoError)   //检查data是否是json格式
        {
            qDebug()<<"json error";
            //return;
        }

        //开始解析
        QJsonObject obj = doc.object();
        //根据data获取picurl
        QString picurl = obj.value("data").toString();

        //插入数据
        sql->inserttable("searchmusiclist", songid ,songmsg, picurl, ApiOfSongLink.arg(songid));
    }

    //发送停止信号
    emit Finishsignal();

}
