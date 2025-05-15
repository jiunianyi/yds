#ifndef CHATUSERWID_HXX
#define CHATUSERWID_HXX

#include <QWidget>
#include"listitembase.hxx"
#include "userdata.hxx"

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    QSize sizeHint() const override {
        return QSize(250, 70); // 返回自定义的尺寸
    }

    void SetInfo(std::shared_ptr<UserInfo>user_info);
    void SetInfo(std::shared_ptr<FriendInfo>friend_info);
    std::shared_ptr<UserInfo> GetUserinfo();
    void updateLastMsg(std::vector<std::shared_ptr<TextChatData>>msgs);

private:
    Ui::ChatUserWid *ui;
    std::shared_ptr<UserInfo>_user_info;
};

#endif // CHATUSERWID_HXX
