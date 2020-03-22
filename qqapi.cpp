#include "qqapi.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
qqapi::qqapi()
{
    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    connect(&manager, &QNetworkAccessManager::finished, this, &qqapi::read_data);

}

void qqapi::search(QString msg)
{
    searchoffset = 1;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void qqapi::nextpage(QString msg)
{
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(msg).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void qqapi::prepage(QString msg)
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

void qqapi::read_data(QNetworkReply *reply)
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
    //根据song获取对象
    QJsonObject songobj = dataobj.value("song").toObject();
    //根据list获取数组
    QJsonArray listarr = songobj.value("list").toArray();

    //遍历数组
    for(int i = 0; i<listarr.count(); i++)
    {
        QJsonObject songobj = listarr.at(i).toObject();    //获取数组中的第i个对象

        //根据albummid获取albummid
        QString albummid = songobj.value("albummid").toString();
        //根据singer获取数组
        QJsonArray singerarr = songobj.value("singer").toArray();
        QString arname; //歌手名
        for(int i = 0; i<singerarr.count(); i++)    //遍历数组
        {
            QJsonObject singerobj = singerarr.at(i).toObject();    //获取数组中的第i个对象
            if(i > 0)
            {
                arname.append(",");
            }
            arname.append(singerobj.value("name").toString());
        }
        //根据songmid获取id
        QString songid = songobj.value("songmid").toString();
        //根据songname获取歌名
        QString songname = songobj.value("songname").toString();
        //拼接歌曲信息
        QString songmsg = songname.append(" - ");
        songmsg.append(arname);

        getqqurl *geturl = new getqqurl;
        geturl->getpurl(songid,i+1);
        connect(geturl, &getqqurl::purlsignal, this, &qqapi::sendfisish);
        //插入数据
        //qDebug()<<"插入数据次数"<<i;
        QSqlQuery query;
        QString sql = QString("insert into %1(songid, songmsg, picurl) values('%2','%3','%4')").arg("searchmusiclist").arg(songid).arg(songmsg).arg(ApiOfPicurl.arg(albummid));
        if(!query.exec(sql))
        {
            qDebug()<<"insert error";
        }
    }
    //发送停止信号
    //emit Finishsignal();

}

void qqapi::sendfisish(int id)
{
    sum +=id;
    //qDebug()<<"sum"<<sum;
    if(sum >= 20)
    {
        //发送停止信号
        sum = 0;
        emit Finishsignal();
    }
}



getqqurl::getqqurl()
{
    connect(&manager, &QNetworkAccessManager::finished, this, &getqqurl::read_data);
}

void getqqurl::getpurl(QString songid, int i)
{
    id = i;
    QUrl url(ApiOfSongPurl.arg(songid));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);

}


void getqqurl::read_data(QNetworkReply *reply)
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
    //根据req_0获取对象
    QJsonObject reqobj = obj.value("req_0").toObject();
    //根据data获取对象
    QJsonObject dataobj = reqobj.value("data").toObject();
    //根据midurlinfo获取数组
    QJsonArray midurlinfoarr = dataobj.value("midurlinfo").toArray();
    //获取数组第一个purl
    QJsonObject urlobj = midurlinfoarr.at(0).toObject();    //获取数组中的第i个对象
    //根据purl获取songurl
    songpurl = urlobj.value("purl").toString();

    //修改数据
    QSqlQuery query;
    QString sql = QString(" UPDATE  searchmusiclist  SET songurl='%1'  WHERE id = '%2'").arg(ApiOfSongurl.arg(songpurl)).arg(id);
    if(!query.exec(sql))
    {
        qDebug()<<"UPDATE error";
    }
    emit purlsignal(1);
}
