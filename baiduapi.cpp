#include "baiduapi.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QEventLoop>

Baiduapi::Baiduapi()
{
    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    connect(&manager, &QNetworkAccessManager::finished, this, &Baiduapi::read_data);
}

void Baiduapi::search(QString msg)
{
    searchoffset = 1;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Baiduapi::nextpage(QString msg)
{
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Baiduapi::prepage(QString msg)
{
    if(searchoffset > 1) {
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

void Baiduapi::read_data(QNetworkReply *reply)
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
    //根据song_list获取数组
    QJsonArray song_listarr = dataobj.value("song_list").toArray();

    //遍历数组
    for(int i = 0; i<song_listarr.count(); i++)
    {
        QJsonObject songobj = song_listarr.at(i).toObject();    //获取数组中的第i个对象

        QString songname = songobj.value("title").toString();    //歌曲名
        QString songid = songobj.value("song_id").toString();    //歌曲id
        QString arname = songobj.value("author").toString();    //歌曲id
        QString picUrl = songobj.value("pic_small").toString();  //专辑/音乐封面
        picUrl.chop(9);
        picUrl.append("w_170,h_170");


        //拼接歌曲信息
        QString songmsg = songname.append(" - ");//歌曲信息
        songmsg.append(arname);
        //插入数据
        sql->inserttable("searchmusiclist", songid ,songmsg, picUrl, ApiOfSongLink.arg(songid));

    }
    //发送停止信号
    emit Finishsignal();
}
