#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QListWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "sql.h"
#include "wangyiapi.h"
#include "qqapi.h"
#include "kugouapi.h"
#include "kuwoapi.h"
#include "miguapi.h"
#include "baiduapi.h"

namespace Ui {
class MusicPlayer;
}

class MusicPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();

    QString formatTime(qint64 timeMilliSeconds);
    void setpic(QString picurl);


private slots:
    void on_pushButton_addfile_clicked();

    void on_pushButton_on_clicked();

    void on_pushButton_prev_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_search_clicked();

    void on_listWidget_searchlist_itemDoubleClicked(QListWidgetItem *item);

    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void read_picdata();
    void dprogress(qint64 rsize, qint64 asize);
    void setmusicPosition(int position);
    void setvolumePosition(int position);
    void updatesearchmusiclist();
    void updatusermsg(int userid, QString username, QString userpic);

    void on_pushButton_prepage_clicked();

    void on_pushButton_nextpage_clicked();

    void on_pushButton_signin_clicked();

private:
    Ui::MusicPlayer *ui;
    //播放器
    QMediaPlayer * mediaplayer;
    //本地播放列表
    QMediaPlaylist *localmusiclist;
    //在线播放列表
    QMediaPlaylist *searchmusiclist;
    //喜欢播放列表
    QMediaPlaylist *lovemusiclist;
    //喜欢播放列表
    QMediaPlaylist *musiclist;

    //管理器
    QNetworkAccessManager manager_pic;
    QByteArray array;

    //图片列表
    QStringList prclist;
    //数据库
    SQL *sql;

    //用户id
    int Userid;

    //网易云 qq
    Wangyiapi *wangyiapi;
    qqapi *qqmapi;
    Kugouapi *kugouapi;
    Kuwoapi *kuwoapi;
    Miguapi *miguapi;
    Baiduapi *baiduapi;

    //搜索页数
    qint32 searchoffset;



};

#endif // MUSICPLAYER_H
