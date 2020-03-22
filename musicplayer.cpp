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

    //添加数据库驱动 创建打开数据库 初始化数据库
    sql = new SQL;
    sql->opensql("music.db");
    //创建searchmusiclist表格
    sql->creattable("searchmusiclist");
    //创建lovemusiclist表格
    sql->creattable("lovemusiclist");

    //初始化音乐api
    wangyiapi = new Wangyiapi;
    qqmapi = new qqapi;
    kuwoapi = new Kuwoapi;
    kugouapi = new Kugouapi;
    miguapi = new Miguapi;
    baiduapi = new Baiduapi;

    connect(mediaplayer, &QMediaPlayer::durationChanged, this, &MusicPlayer::updateDuration); //当前播放音乐改变
    connect(mediaplayer, &QMediaPlayer::positionChanged, this, &MusicPlayer::updatePosition);
    connect(ui->horizontalSlider_music, &QAbstractSlider::valueChanged, this, &MusicPlayer::setmusicPosition);
    connect(ui->horizontalSlider_volume, &QAbstractSlider::valueChanged, this, &MusicPlayer::setvolumePosition);
    connect(wangyiapi, &Wangyiapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
    connect(qqmapi, &qqapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
    connect(kuwoapi, &Kuwoapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
    connect(kugouapi, &Kugouapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
    connect(miguapi, &Miguapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
    connect(baiduapi, &Baiduapi::Finishsignal, this, &MusicPlayer::updatesearchmusiclist);
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
    //清空歌曲列表 清空各种数据
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();
    sql->deletetable("searchmusiclist");

    //搜索
    searchoffset = 0;   //默认为第一页
    QString songkey = ui->lineEdit_searchsong->text();
    if(ui->comboBox_api->currentIndex() < 2) wangyiapi->search(songkey);//网易云
    else if(ui->comboBox_api->currentIndex() == 2) qqmapi->search(songkey);//qq音乐
    else if(ui->comboBox_api->currentIndex() == 3) kugouapi->search(songkey);//酷狗音乐
    else if(ui->comboBox_api->currentIndex() == 4) kuwoapi->search(songkey);//酷我音乐
    else if(ui->comboBox_api->currentIndex() == 5) miguapi->search(songkey);//咪咕音乐
    else if(ui->comboBox_api->currentIndex() == 6) baiduapi->search(songkey);//百度音乐
    else return;


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
    if(current > 0) {
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

//搜索完成 更新列表
void MusicPlayer::updatesearchmusiclist()
{
    //查询数据
    QSqlQuery query;
    query.exec("select * from searchmusiclist");
    if(!query.exec())
    {
        qDebug()<<"select error";
    }
    else
    {
        while(query.next())
        {
            QString id = query.value(1).toString();
            QString songmsg = query.value(2).toString();
            QString picurl = query.value(3).toString();
            QString songurl = query.value(4).toString();
            //在搜索列表中显示
            //ui->listWidget_searchlist->addItem(songmsg);
            MusicWinItem *musicitem = new MusicWinItem;
            musicitem->setsongmsg(songmsg);
            musicitem->setpic(picurl);
            QListWidgetItem * item = new QListWidgetItem;
            item->setSizeHint(musicitem->sizeHint());
            ui->listWidget_searchlist->addItem(item);
            ui->listWidget_searchlist->setItemWidget(item,musicitem);
            //将图片连接添加到图片列表
            prclist.append(picurl);
            //将搜索的歌曲添加进搜索列表
            searchmusiclist->setPlaybackMode(QMediaPlaylist::Loop);
            searchmusiclist->addMedia(QUrl(songurl));
            //qDebug()<<"插入数据库"<<songmsg;
            //qDebug()<<QString("songid:%1    songmsg:%2    picurl:%3    songurl:%4").arg(id).arg(songmsg).arg(picurl).arg(songurl);
        }
    }
}



void MusicPlayer::on_pushButton_prepage_clicked()
{
    if(searchoffset > 0) {
        searchoffset--;
    }
    else {
        return;
    }
    //清空歌曲列表 清空各种数据
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();
    sql->deletetable("searchmusiclist");

    QString songkey = ui->lineEdit_searchsong->text();
    if(ui->comboBox_api->currentIndex() < 2) wangyiapi->prepage(songkey);//网易云
    else if(ui->comboBox_api->currentIndex() == 2) qqmapi->prepage(songkey);//qq音乐
    else if(ui->comboBox_api->currentIndex() == 3) kugouapi->prepage(songkey);//酷狗音乐
    else if(ui->comboBox_api->currentIndex() == 4) kuwoapi->prepage(songkey);//酷我音乐
    else if(ui->comboBox_api->currentIndex() == 5) miguapi->prepage(songkey);//咪咕音乐
    else if(ui->comboBox_api->currentIndex() == 6) baiduapi->prepage(songkey);//百度音乐
    else return;

}

void MusicPlayer::on_pushButton_nextpage_clicked()
{
    searchoffset++;
    //清空歌曲列表 清空各种数据
    ui->listWidget_searchlist->clear();
    searchmusiclist->clear();
    prclist.clear();
    sql->deletetable("searchmusiclist");

    QString songkey = ui->lineEdit_searchsong->text();
    if(ui->comboBox_api->currentIndex() < 2) wangyiapi->nextpage(songkey);//网易云
    else if(ui->comboBox_api->currentIndex() == 2) qqmapi->nextpage(songkey);//qq音乐
    else if(ui->comboBox_api->currentIndex() == 3) kugouapi->nextpage(songkey);//酷狗音乐
    else if(ui->comboBox_api->currentIndex() == 4) kuwoapi->nextpage(songkey);//酷我音乐
    else if(ui->comboBox_api->currentIndex() == 5) miguapi->nextpage(songkey);//咪咕音乐
    else if(ui->comboBox_api->currentIndex() == 6) baiduapi->nextpage(songkey);//百度音乐
    else return;
}
