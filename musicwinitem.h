#ifndef MUSICWINITEM_H
#define MUSICWINITEM_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Ui {
class MusicWinItem;
}

class MusicWinItem : public QWidget
{
    Q_OBJECT

public:
    explicit MusicWinItem(QWidget *parent = nullptr);
    ~MusicWinItem();
    void setsongmsg(QString songmsg);
    void setpic(QString picurl);

private slots:
    void read_data();
    void dprogress(qint64 rsize, qint64 asize);

private:
    Ui::MusicWinItem *ui;
    QNetworkAccessManager manager;
    QByteArray array;
};

#endif // MUSICWINITEM_H
