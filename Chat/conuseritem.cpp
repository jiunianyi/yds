#include "conuseritem.hxx"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250,70);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthInfo> auth_info)
{
    auto _info = std::make_shared<UserInfo>(auth_info);
    QPixmap pixmap(_info->_icon);
    //设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto _info = std::make_shared<UserInfo>(auth_rsp);
    QPixmap pixmap(_info->_icon);

    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(_info->_name);
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
     _infoooo = std::make_shared<UserInfo>(uid,name,icon);
    QPixmap pixmap(_infoooo->_icon);

    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(_infoooo->_name);
}

void ConUserItem::ShowRedPoint(bool show)
{
    if(show)
    {
        ui->red_point->show();
    }
    else
    {
        ui->red_point->hide();
    }
}

std::shared_ptr<UserInfo> ConUserItem::GetInfo()
{
    return _infoooo;
}


