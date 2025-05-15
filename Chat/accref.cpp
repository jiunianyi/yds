#include "accref.hxx"
#include "global.h"
#include "ui_accref.h"
#include"huat.hxx"
#include"opus.hxx"
#include"selectbtn.hxx"
#include"udpmgr.hxx"
accref::accref(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::accref)
{
    ui->setupUi(this);
    qDebug()<<"211";
    connect(this,&accref::qi_start,&opus::getInstance(),&opus::start);
}

accref::~accref()
{
    delete ui;
}

void accref::on_pushButton_2_clicked()
{
    this->hide();
    emit ref_yubyin();
}

void accref::swad()
{
    this->show();
}


void accref::on_pushButton_clicked()
{
    this->hide();
    HuaT::getInstance().show();
    emit accp_yuyin();
    int id = ID_YUYIN;
   // int uid = selectbtn::getInstance().touid;
    qDebug()<<udpmgr::getInstance().fromuid<<"sssssssss";
    emit qi_start(id,udpmgr::getInstance().fromuid);
}

