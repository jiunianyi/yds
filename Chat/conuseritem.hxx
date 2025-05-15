#ifndef CONUSERITEM_HXX
#define CONUSERITEM_HXX

#include <QWidget>
#include"listitembase.hxx"
#include"userdata.hxx"
#include<memory>
namespace Ui {
class ConUserItem;
}

class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    QSize sizeHint() const override;
    void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(int uid, QString name, QString icon);
    void ShowRedPoint(bool show = false);
    std::shared_ptr<UserInfo> GetInfo();
private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _infoooo;

};

#endif // CONUSERITEM_HXX
