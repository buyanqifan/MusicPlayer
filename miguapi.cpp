#include "miguapi.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QEventLoop>

Miguapi::Miguapi()
{
    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    connect(&manager, &QNetworkAccessManager::finished, this, &Miguapi::read_data);
}

void Miguapi::search(QString msg)
{
    searchoffset = 1;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Miguapi::nextpage(QString msg)
{
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void Miguapi::prepage(QString msg)
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

void Miguapi::read_data(QNetworkReply *reply)
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
    //根据resultList获取数组
    QJsonArray resultListarr = dataobj.value("resultList").toArray();
    //遍历数组
    for(int i = 0; i<resultListarr.count(); i++)
    {
        QJsonArray songarr = resultListarr.at(i).toArray();    //获取数组中的第i个数组
        //遍历数组
        for(int i = 0; i<songarr.count(); i++)
        {
            //获取数组第一个对象
            QJsonObject songobj = songarr.at(i).toObject();    //获取数组中的第i个对象

            //根据singers获取数组
            QJsonArray singersarr = songobj.value("singers").toArray();
            //遍历数组
            QString arname; //歌手名
            for(int i = 0; i<singersarr.count(); i++)
            {
                QJsonObject alnameobj = singersarr.at(i).toObject();
                if(i > 0)
                {
                    arname.append(",");
                }
                arname.append(alnameobj.value("name").toString());
            }

            //根据copyrightId获取songid
            QString songid = songobj.value("copyrightId").toString();
            //根据name获取songname
            QString songname = songobj.value("name").toString();

            //拼接歌曲信息
            QString songmsg = songname.append(" - ");//歌曲信息
            songmsg.append(arname);

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
            qDebug()<<songmsg;
            //插入数据
            sql->inserttable("searchmusiclist", songid ,songmsg, picurl, ApiOfSongLink.arg(songid));
        }

    }
    //发送停止信号
    emit Finishsignal();
}
