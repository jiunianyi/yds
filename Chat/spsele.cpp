#include "spsele.hxx"
#include "ui_spsele.h"
#include"video.hxx"
spsele::spsele(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::spsele)
{
    ui->setupUi(this);
}

spsele::~spsele()
{
    delete ui;
}

void spsele::swsps()
{
    this->show();
}

void spsele::on_pushButton_2_clicked()
{
    this->hide();
    emit accship();
    video::getInstance().show();
    video::getInstance().sendship();
}

