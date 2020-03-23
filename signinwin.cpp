#include "signinwin.h"
#include "ui_signinwin.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>

SigninWin::SigninWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SigninWin)
{
    ui->setupUi(this);
}

SigninWin::~SigninWin()
{
    delete ui;
}

void SigninWin::on_pushButton_signin_clicked()
{
    //获取账号密码
    QString phone = ui->lineEditp_phone->text();
    QString password = ui->lineEdit_password->text();

    QUrl url(Signinapi.arg(phone).arg(password));
    QNetworkRequest request(url);
    //发送请求
    QNetworkReply* reply = manager.get(request);
    //等待读取数据
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    //读取数据
    QString data = reply->readAll();
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
    //根据profile获取对象
    QJsonObject profileobj = obj.value("profile").toObject();
    if(profileobj.isEmpty())//账号密码错误
    {
        ui->lineEdit_password->clear();
        QMessageBox msgBox;
        msgBox.setText("账号或密码错误，请重新输入");
        msgBox.exec();
        return;
    }
    //根据avatarUrl获取头像
    QString userpic = profileobj.value("avatarUrl").toString();
    //根据nickname获取用户名
    QString username = profileobj.value("nickname").toString();
    //根据userId获取用户id
    int userid = profileobj.value("nickname").toInt();

    emit signinsignal(userid,username,userpic);
    this->close();

}

void SigninWin::on_pushButton_close_clicked()
{
    this->close();
}
