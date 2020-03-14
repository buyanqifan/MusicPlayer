#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QListWidget>


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

    void read_data(QNetworkReply *reply);

    void on_listWidget_searchlist_itemDoubleClicked(QListWidgetItem *item);

    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void read_picdata();
    void dprogress(qint64 rsize, qint64 asize);
    void setmusicPosition(int position);
    void setvolumePosition(int position);

    void on_pushButton_prepage_clicked();

    void on_pushButton_nextpage_clicked();

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
    QNetworkAccessManager manager;
    QNetworkAccessManager manager_pic;
    QByteArray array;

    //图片列表
    QStringList prclist;

    //音乐搜索API
    //参数%1：搜索关键字
    //参数%2：页数
    const QString ApiOfSearch = "https://api.imjad.cn/cloudmusic/?type=search&s=%1&offset=%2";
    qint32 searchoffset;    //搜索页数
    //歌曲链接API
    //参数%1：歌曲id
    const QString ApiOfSongLink = "https://music.163.com/song/media/outer/url?id=%1.mp3";

};

#endif // MUSICPLAYER_H
