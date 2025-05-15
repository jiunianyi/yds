#include "huat.hxx"
#include "ui_huat.h"
#include"clickedlabel.hxx"
HuaT::HuaT(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HuaT)
{
    ui->setupUi(this);
    QPixmap pix(":/res/AI.jpg");
    QPixmap pixmap(":/res/GD.jpg");
    ui->label_2->setPixmap(pixmap); // 将图片设置到QLabel上
    QPixmap scaledPixmap = pixmap.scaled( ui->label_2->size(), Qt::KeepAspectRatio); // 将图片缩放到label的大小
    ui->label_2->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
    ui->label_2->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
    connect(ui->label_2,&ClickedLabel::clicked,this,&HuaT::stop);
}

HuaT::~HuaT()
{
    delete ui;
}


void HuaT::stop()
{
    this->hide();
    emit guaduan();
    emit sendgd();
}


