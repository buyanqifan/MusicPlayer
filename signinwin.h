#ifndef SIGNINWIN_H
#define SIGNINWIN_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Ui {
class SigninWin;
}

class SigninWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit SigninWin(QWidget *parent = nullptr);
    ~SigninWin();

private slots:
    void on_pushButton_signin_clicked();

    void on_pushButton_close_clicked();

signals:
    void signinsignal(int userid, QString username, QString userpic);

private:
    Ui::SigninWin *ui;

    const QString Signinapi = "http://musicapi.leanapp.cn/login/cellphone?phone=%1&password=%2";

    //管理器
    QNetworkAccessManager manager;
};

#endif // SIGNINWIN_H
