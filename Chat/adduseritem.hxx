#ifndef ADDUSERITEM_HXX
#define ADDUSERITEM_HXX

#include <QWidget>
#include"listitembase.hxx"
namespace Ui {
class AddUserItem;
}

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();
    QSize  sizeHint()const override{
        return QSize(250,70);//自定义
    }

private:
    Ui::AddUserItem *ui;
};

#endif // ADDUSERITEM_HXX
