#include "loadingdlg.hxx"
#include "ui_loadingdlg.h"
#include<QMovie>

LoadingDlg::LoadingDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);//设置背景为透明
    setFixedSize(parent->size());//设置对话框为全屏尺寸

    QMovie *movie = new QMovie(":/res/loading.gif");
    ui->loading_lb->setMovie(movie);
    movie->start();
}

LoadingDlg::~LoadingDlg()
{
    delete ui;
}
