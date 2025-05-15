#include "findsuccessdia.hxx"
#include "ui_findsuccessdia.h"
#include<QDir>
#include "applyfriend.hxx"
#include"clickedbtn.hxx"
FindSuccessDia::FindSuccessDia(QWidget *parent)
    : QDialog(parent),_parent(parent),
     ui(new Ui::FindSuccessDia)
{
    ui->setupUi(this);
    //设置对话框标题
    setWindowTitle("添加");
    //隐藏对话框标题栏
    setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
    //获取当前应用程序路径
    QString app_path = QCoreApplication::applicationDirPath();
    qDebug()<<app_path;
    QString pix_path = QDir::toNativeSeparators(app_path +
                                                QDir::separator() + "static"+QDir::separator()+"head_1.jpg");
    //qDebug()<<pix_path;
  /*  if (!QFile::exists(pix_path)) {
        qDebug() << "File does not exist:" << pix_path;
    }
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);*/
    ui->add_firend_btn->SetState("normal","hover","press");
    qDebug()<<"头像设置";
    this->setModal(true);

}

FindSuccessDia::~FindSuccessDia()
{
    delete ui;
}

void FindSuccessDia::SetSearchInfo(std::shared_ptr<SearchInfo> si)
{
    ui->name_lb->setText(si->_name);
    QPixmap head_pix(si->_icon);
    head_pix = head_pix.scaled(ui->head_lb->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_lb->setPixmap(head_pix);
    _si=si;
}
//添加好又界面

void FindSuccessDia::on_add_firend_btn_clicked()
{
    qDebug()<<"弹出界面";
    this->hide();
    auto applyfriend = new ApplyFriend(_parent);
    applyfriend->SetSearchInfo(_si);
    applyfriend->setModal(true);
    applyfriend->show();
}

