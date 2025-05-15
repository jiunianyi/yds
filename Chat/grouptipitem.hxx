#ifndef GROUPTIPITEM_HXX
#define GROUPTIPITEM_HXX

#include "listitembase.hxx"
#include <QWidget>

namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();
    QSize sizeHint() const ;
    void SetGroupTip(QString str);
private:
    QString _tip;
    Ui::GroupTipItem *ui;
};

#endif // GROUPTIPITEM_HXX
