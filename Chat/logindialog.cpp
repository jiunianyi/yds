#include "logindialog.hxx"
#include "ui_logindialog.h"
#include<QDebug>
#include<qpainter.h>
#include<QPainterPath>
#include"httpmgr.hxx"
#include"tcpmgr.hxx"
#include"udpmgr.hxx"
loginDialog::loginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    connect(ui->reg_btn,&QPushButton::clicked,this,&loginDialog::switchRegister);

    ui->forget_lable->SetState("normal","hover","","selected","selected_hover","");
    connect(ui->forget_lable, &ClickedLabel::clicked, this, &loginDialog::slot_forget_pwd);

    ui->err_tip->clear();
    initHead();
    initHttpHandlers();
    //连接登录回包信号
    connect(HttpMgr::getinstance().get(),&HttpMgr::sig_login_mod_finish,this, &loginDialog::slot_login_mod_finish);

    //连接tcp连接请求的信号和槽函数
    connect(this, &loginDialog::sig_connect_tcp, TcpMgr::getinstance().get(), &TcpMgr::slot_tcp_connect);
    //连接udp连接请求的信号和槽函数
    connect(this,&loginDialog::sig_connect_udp,&udpmgr::getInstance(),&udpmgr::slot_udp_connect);
    //连接tcp管理者发出的连接成功信号
   connect(TcpMgr::getinstance().get(), &TcpMgr::sig_con_success, this, &loginDialog::slot_tcp_con_finish);
    //连接tcp失败
   connect(TcpMgr::getinstance().get(),&TcpMgr::sig_con_failed,this,&::loginDialog::slot_login_failed);
}

loginDialog::~loginDialog()
{
    qDebug()<<"destruct LoginDlg";
    delete ui;
}

void loginDialog::initHttpHandlers()
{
    //注册获取登录回报
    qDebug()<<"666";
    qDebug()<<"666";
    _handlers.insert(RedId::ID_LOGIN_USER,[this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showtip(tr("参数错误"),false) ;
            enableBtn(true);
            return;
        }
        //qDebug()<<"4";
        auto email = jsonObj["email"].toString();
        //发送信号通知tcpmrg发送长连接
        ServerInfo si;
        si.uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();
        si.name = jsonObj["name"].toString();

        _uid = si.uid;
        _token = si.Token;
        qDebug()<<"Host is"<<si.Host<<"name si"<<si.name;

        qDebug()<<"email is"<<email<<"uuid is"<<si.uid<<"port is:"<<si.Port<<"token is: "<<si.Token;
        emit sig_connect_tcp(si);
        emit sig_connect_udp(si);
    });
}

void loginDialog::initHead()
{
    //加载图片
    QPixmap originalPixmap("://res/head_1.jpg");
    //设置图片自动缩放
    qDebug()<<originalPixmap.size()<<ui->head_label->size();
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    //创建一个和原图片大小相同的Qpixmap，用于绘制圆角图片
    QPixmap  roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent);//通明

    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    //使用qpainterpath设置圆角
    QPainterPath path;
    path.addRoundedRect(0,0,originalPixmap.width(),originalPixmap.height(),10,10);
    painter.setClipPath(path);

    //将原图绘制到roundedpixmap上
    painter.drawPixmap(0,0,originalPixmap);

    //将图片设置到label上
    ui->head_label->setPixmap(roundedPixmap);

}

bool loginDialog::checkUserValid()
{
    auto user = ui->ure_edit->text();
    if(user.isEmpty()){
        qDebug() << "User empty " ;
        AddTipErr(TipErr::TIP_USER_ERR,tr("用户名不能为空"));
        return false;
    }
    return true;
}

bool loginDialog::checkPwdValid()
{
    auto pwd = ui->path_edit->text();
    if(pwd.length()<6||pwd.length()>15)
    {
        qDebug()<<"pwd error";
        AddTipErr(TipErr::TIP_PWD_ERR,tr("密码长度应为6~15"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

void loginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te]=tips;
    showtip(tips,false);
}

void loginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty())
    {
        ui->err_tip->clear();
        return;
    }
    showtip(_tip_errs.first(),false);

}

void loginDialog::showtip(QString str, bool b_ok)
{
    if(b_ok)
    {
        ui->err_tip->setProperty("state","normal");
    }
    else{
        ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}

bool loginDialog::enableBtn(bool enabled)
{
    ui->login_btn->setEnabled(enabled);
    ui->reg_btn->setEnabled( enabled);
    return true;
}

void loginDialog::slot_forget_pwd()
{
    qDebug()<<"slot forget pwd";
    emit switchReset();
}

void loginDialog::on_login_btn_clicked()
{
    qDebug()<<"login btn clicked";
    if(checkUserValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return ;
    }
    auto user = ui->ure_edit->text();
    auto pwd = ui->path_edit->text();
    //发送http请求登录
    QJsonObject json_obj;
    json_obj["user"] = user;
    json_obj["passwd"] = xorString(pwd);
    qDebug()<<gate_url_prefix<<"测试";
    HttpMgr::getinstance()->posthttpreq(QUrl(gate_url_prefix+"/user_login"),
                                        json_obj, RedId::ID_LOGIN_USER,Modules::LOGINMOD);
}

void loginDialog::slot_login_mod_finish(RedId id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showtip(tr("网络请求错误"),false);
        return;
    }
    qDebug()<<"6666666";
    // 解析 JSON 字符串,res需转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    //json解析错误
    if(jsonDoc.isNull()){
        showtip(tr("json解析错误"),false);
        return;
    }

    //json解析错误
    if(!jsonDoc.isObject()){
        showtip(tr("json解析错误"),false);
        return;
    }


    //调用对应的逻辑,根据id回调。
    _handlers[id](jsonDoc.object());

    return;
}
void loginDialog::slot_tcp_con_finish(bool bsuccess)
{
    qDebug()<<"fghjk";
    if(bsuccess){
        showtip(tr("聊天服务连接成功，正在登录..."),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;
        QJsonDocument doc(jsonObj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        emit TcpMgr::getinstance()->sig_send_data(RedId::ID_CHAT_LOGIN, jsonString);
    }else{
        showtip(tr("网络异常"),false);
        enableBtn(true);
    }
}
void loginDialog::slot_login_failed(int err)
{
    QString result = QString("登陆失败,err is %1").arg(err);
    showtip(result,false);
    enableBtn(true);
}
