#include "listitembase.hxx"
#include<QStyleOption>
#include<QPainter>

ListItemBase::ListItemBase(QWidget * parent):QWidget(parent)
{

}

void ListItemBase::SetItemType(ListItemType itemType)
{
    _itemType = itemType;
}

ListItemType ListItemBase::GetItemType()
{
    return _itemType;
}

void ListItemBase::painEvent(QPaintEvent *event)
{
    QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

