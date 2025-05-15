#ifndef APPLYFRIENDITEM_HXX
#define APPLYFRIENDITEM_HXX

#include <QWidget>
#include"listitembase.hxx"
#include"userdata.hxx"
#include<memory>

namespace Ui {
class ApplyFriendItem;
}

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT
public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();
    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    void ShowAddBtn(bool bshow);
    QSize sizeHint() const override {
        return QSize(250, 80); // 返回自定义的尺寸
    }
    int GetUid();
private:
    Ui::ApplyFriendItem *ui;
    std::shared_ptr<ApplyInfo> _apply_info;
    bool _added;
signals:
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);
};

#endif // APPLYFRIENDITEM_HXX
