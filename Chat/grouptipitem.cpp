#include "grouptipitem.hxx"
#include "ui_grouptipitem.h"

GroupTipItem::GroupTipItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::GroupTipItem)
{
    ui->setupUi(this);
}

GroupTipItem::~GroupTipItem()
{
    delete ui;
}

QSize GroupTipItem::sizeHint() const
{
    return QSize(250,25);
}

void GroupTipItem::SetGroupTip(QString str)
{
    ui->label->setText(str);
}
