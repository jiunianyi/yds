#include "selectbtn.hxx"
#include "ui_selectbtn.h"
#include"usermgr.h"
#include<QJsonDocument>
#include<QThread>
selectbtn::selectbtn(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::selectbtn)
{
    ui->setupUi(this);
   // QObject::connect(this,&selectbtn::sig_yuyin_req,&udpmgr::getInstance(),&udpmgr::send_yuyin_connect);
}

selectbtn::~selectbtn()
{
    delete ui;
}

void selectbtn::on_pushButton_2_clicked()
{
    ui->label->setText("等待对方接听");
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    QJsonObject object;
    object["fromuid"] = user_info->_uid;
    object["touid"] = touid;
    qDebug()<<"touid等于"<<touid;
    object["id"] = ID_SELECT_YUYIN_REQ;
    QJsonDocument doc(object);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    emit sig_yuyin_req(jsonData);
}


void selectbtn::on_cancel_btn_clicked()
{
    this->hide();
}

void selectbtn::slot_close_sele()
{
    ui->label->setText("对方已拒绝");
       QThread::sleep(1); // 暂停 1 秒
    this->hide();
}

