#include "musicplayer.h"
#include "ui_musicplayer.h"
#include "musicwinitem.h"
#include <QFileDialog>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>


MusicPlayer::MusicPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    //播放器和播放列表初始化
    mediaplayer = new QMediaPlayer;
    localmusiclist = new QMediaPlaylist;
    searchmusiclist = new QMediaPlaylist;
    lovemusiclist = new QMediaPlaylist;
    musiclist = new QMediaPlaylist;


    //设置播放模式：循环播放
    musiclist->setPlaybackMode(QMediaPlaylist::Loop);
    mediaplayer->setPlaylist(musiclist);

    //初始化音量条
    ui->horizontalSlider_volume->setRange(0, 100);
     ui->horizontalSlider_volume->setValue(30);
    mediaplayer->setVolume(30);

    connect(&manager, &QNetworkAccessManager::finished, this, &MusicPlayer::read_data);
    connect(mediaplayer, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateDuration); //当前播放音乐改变
    connect(mediaplayer, &QMediaPlayer::positionChanged, this, &MusicPlayer::updatePosition);
    connect(ui->horizontalSlider_music, &QAbstractSlider::valueChanged, this, &MusicPlayer::setmusicPosition);
    connect(ui->horizontalSlider_volume, &QAbstractSlider::valueChanged, this, &MusicPlayer::setvolumePosition);
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
}



void MusicPlayer::on_pushButton_addfile_clicked()
{
    //打开文件
    QString filename = QFileDialog::getOpenFileName();
    //添加进播放目录
    localmusiclist->addMedia(QUrl::fromLocalFile(filename));
    ui->listWidget_filelist->addItem(filename);

}

void MusicPlayer::on_pushButton_on_clicked()
{
    //判断当前播放状态
    if(mediaplayer->state() == QMediaPlayer::PlayingState)  //正在播放
    {
        mediaplayer->pause();
    }
    else {      //正在停止播放
        mediaplayer->play();
    }
    int current = musiclist->currentIndex();  //获取当前歌曲位置
    ui->listWidget_filelist->setCurrentRow(current);
}

void MusicPlayer::on_pushButton_prev_clicked()
{
    int row = musiclist->mediaCount();  //获取列表歌曲数
    int current = musiclist->currentIndex();  //获取当前歌曲位置
    if(current == 0)    //如果是第一首则跳到最后一首
    {
        current = row - 1;
    }
    else {
        current--;
    }

    musiclist->setCurrentIndex(current);
    ui->listWidget_filelist->setCurrentRow(current);
    mediaplayer->play();

}

void MusicPlayer::on_pushButton_next_clicked()
{
    int row = musiclist->mediaCount();  //获取列表歌曲数
    int current = musiclist->currentIndex();  //获取当前歌曲位置
    if(current == row-1)    //如果是最后一首则跳到第一首
    {
        current = 0;
    }else {
        current++;
    }

    musiclist->setCurrentIndex(current);
    ui->listWidget_filelist->setCurrentRow(current);
    mediaplayer->play();
}

void MusicPlayer::on_pushButton_search_clicked()
{
    //清空歌曲列表
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();

    QString songkey = ui->lineEdit_searchsong->text();
    searchoffset = 0;   //默认为第一页
    //获取请求对象
    QUrl url(ApiOfSearch.arg(songkey).arg(searchoffset));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);

}

void MusicPlayer::read_data(QNetworkReply *reply)
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
        QString songmsg = songname.append(" - ");
        songmsg.append(arname);
        //在搜索列表中显示
        //ui->listWidget_searchlist->addItem(songmsg);
        MusicWinItem *musicitem = new MusicWinItem;
        musicitem->setsongmsg(songmsg);
        musicitem->setpic(picUrl);
        QListWidgetItem * item = new QListWidgetItem;
        item->setSizeHint(musicitem->sizeHint());
        ui->listWidget_searchlist->addItem(item);
        ui->listWidget_searchlist->setItemWidget(item,musicitem);

        //将图片连接添加到图片列表
        prclist.append(picUrl);
        //将搜索的歌曲添加进搜索列表
        searchmusiclist->setPlaybackMode(QMediaPlaylist::Loop);
        searchmusiclist->addMedia(QUrl(ApiOfSongLink.arg(songid)));
        qDebug()<<QUrl(ApiOfSongLink.arg(songid));
    }

}

//双击播放搜索列表
void MusicPlayer::on_listWidget_searchlist_itemDoubleClicked(QListWidgetItem *item)
{
    MusicWinItem * win = qobject_cast<MusicWinItem *>(ui->listWidget_searchlist->itemWidget(item));
    qDebug()<<win->getsongmsg();
    int currentrow = ui->listWidget_searchlist->currentRow();

    //设置封面
    setpic(prclist.at(currentrow));

    //更新播放列表
    musiclist->clear();
    for(int i = 0; i<searchmusiclist->mediaCount(); i++)
    {
        musiclist->addMedia(searchmusiclist->media(i));
    }
    //mediaplayer->setPlaylist(musiclist);
    musiclist->setCurrentIndex(currentrow);
    mediaplayer->play();
}


//当前播放歌曲改变
void MusicPlayer::updateDuration(qint64 duration)
{
    ui->horizontalSlider_music->setRange(0, static_cast<int>(duration));
    ui->label_duration->setText(formatTime(duration));

    int current = musiclist->currentIndex();  //获取当前歌曲位置
    //设置封面
    if(!prclist.isEmpty()) {
        setpic(prclist.at(current));
    }
    //更新播放位置
    ui->listWidget_searchlist->setCurrentRow(current);

}

//当前进度条改变
void MusicPlayer::updatePosition(qint64 position)
{
    ui->horizontalSlider_music->setValue(static_cast<int>(position));
    ui->label_nowduration->setText(formatTime(position));

}

QString MusicPlayer::formatTime(qint64 timeMilliSeconds)
{
    qint64 seconds = timeMilliSeconds / 1000;
    const qint64 minutes = seconds / 60;
    seconds -= minutes * 60;
    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void MusicPlayer::setpic(QString picurl)
{
    QUrl url(picurl);
    QNetworkRequest request(url);

    QNetworkReply* reply = manager_pic.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &MusicPlayer::read_picdata);

    connect(reply, &QNetworkReply::downloadProgress, this, &MusicPlayer::dprogress);

}

void MusicPlayer::read_picdata()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray arry = reply->readAll();
    array.append(arry);

}

void MusicPlayer::dprogress(qint64 rsize, qint64 asize)
{
    //如果读取图片数据为空，重新读取
    if(array.isNull())
    {
        return read_picdata();
    }
    if(rsize == asize && asize!=0)
    {
        QPixmap mmp;
        mmp.loadFromData(array);
        QSize picSize(170,170);
        mmp = mmp.scaled(picSize, Qt::KeepAspectRatio);
        ui->label_pic->setPixmap(mmp);
        array.clear();
    }
}

//拖动进度条
void MusicPlayer::setmusicPosition(int position)
{
    if (qAbs(mediaplayer->position() - position) > 99)  //防止进度条自己走动时修改进度条，出现卡顿
    mediaplayer->setPosition(position);
}

//改变音量
void MusicPlayer::setvolumePosition(int position)
{
    mediaplayer->setVolume(position);
}



void MusicPlayer::on_pushButton_prepage_clicked()
{
    if(searchoffset > 0) {
        searchoffset--;
    }
    else {
        return;
    }
    //清空歌曲列表
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();

    QString songkey = ui->lineEdit_searchsong->text();


    //获取请求对象
    QUrl url(ApiOfSearch.arg(songkey).arg(searchoffset*20));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}

void MusicPlayer::on_pushButton_nextpage_clicked()
{
    //清空歌曲列表
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();

    QString songkey = ui->lineEdit_searchsong->text();
    searchoffset++;
    //获取请求对象
    QUrl url(ApiOfSearch.arg(songkey).arg(searchoffset*20));
    QNetworkRequest request(url);
    //发送请求
    manager.get(request);
}
