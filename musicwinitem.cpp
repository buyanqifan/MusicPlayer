#include "musicwinitem.h"
#include "ui_musicwinitem.h"
#include "ui_musicplayer.h"
#include <QElapsedTimer>

MusicWinItem::MusicWinItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicWinItem)
{
    ui->setupUi(this);
}

MusicWinItem::~MusicWinItem()
{
    delete ui;
}

void MusicWinItem::setsongmsg(QString songmsg)
{
    ui->label_songmsg->setText(songmsg);
}

void MusicWinItem::setpic(QString picurl)
{

    QUrl url(picurl);
    QNetworkRequest request(url);

    QNetworkReply* reply = manager.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &MusicWinItem::read_data);

    connect(reply, &QNetworkReply::downloadProgress, this, &MusicWinItem::dprogress);

}

QString MusicWinItem::getsongmsg()
{
    return ui->label_songmsg->text();
}

void MusicWinItem::read_data()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray arry = reply->readAll();
    array.append(arry);

}

void MusicWinItem::dprogress(qint64 rsize, qint64 asize)
{
    if(array.isNull())
    {
        return read_data();
    }
    if(rsize == asize && asize!=0)
    {
        QPixmap mmp;
        mmp.loadFromData(array);
        QSize picSize(80,80);
        mmp = mmp.scaled(picSize, Qt::KeepAspectRatio);
        ui->label_pic->setPixmap(mmp);
        array.clear();
    }
}


