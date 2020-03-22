#include "wangyiapi.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>

Wangyiapi::Wangyiapi()
{
    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    connect(&manager, &QNetworkAccessManager::finished, this, &Wangyiapi::read_data);
}

void Wangyiapi::search(QString msg)
{
    searchoffset = 0;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Wangyiapi::nextpage(QString msg)
{
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset*20));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Wangyiapi::prepage(QString msg)
{
    if(searchoffset > 0) {
        searchoffset--;
    }
    else {
        return;
    }
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset*20));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Wangyiapi::read_data(QNetworkReply *reply)
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
    //根据result获取对象
    QJsonObject resultobj = obj.value("result").toObject();
    //根据songs获取数组
    QJsonArray songsarr = resultobj.value("songs").toArray();

    //遍历数组
    for(int i = 0; i<songsarr.count(); i++)
    {
        QJsonObject songobj = songsarr.at(i).toObject();    //获取数组中的第i个对象

        QString songname = songobj.value("name").toString();    //歌曲名
        int songid = songobj.value("id").toInt();    //歌曲id
        //根据al获取对象
        QJsonObject alobj = songobj.value("al").toObject();
        QString picUrl = alobj.value("picUrl").toString();  //专辑/音乐封面

        //根据ar获取数组
        QJsonArray ararr = songobj.value("ar").toArray();
        //遍历数组
        QString arname; //歌手名
        for(int i = 0; i<ararr.count(); i++)
        {
            QJsonObject alnameobj = ararr.at(i).toObject();
            if(i > 0)
            {
                arname.append(",");
            }
            arname.append(alnameobj.value("name").toString());

        }
        //拼接歌曲信息
        QString songmsg = songname.append(" - ");//歌曲信息
        songmsg.append(arname);
        //插入数据
        sql->inserttable("searchmusiclist", QString::number(songid) ,songmsg, picUrl, ApiOfSongLink.arg(songid));

    }
    //发送停止信号
    emit Finishsignal();
}


