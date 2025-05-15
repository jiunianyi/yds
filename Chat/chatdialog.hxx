#ifndef CHATDIALOG_HXX
#define CHATDIALOG_HXX

#include <QDialog>
#include"global.h"
#include "qlistwidget.h"
#include"statewidget.hxx"
#include"userdata.hxx"
namespace Ui {
class chatDialog;
}

class chatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit chatDialog(QWidget *parent = nullptr);
    ~chatDialog();
    void addChatUserList();
    void ClearLabelState(StateWidget *lb);
protected:
    bool eventFilter(QObject *watched,QEvent *event);
    void handleGlobalMousePress(QMouseEvent*event);
private:
    void ShowSerach(bool bsersch = false);
    void AddLBGroup(StateWidget *lb);
    Ui::chatDialog *ui;

    ChatUIMode _mode;
    ChatUIMode _state;
    bool _b_loading;
    QList<StateWidget*>_lb_list;
    QMap<int,QListWidgetItem*>_chat_items_added;
    int _cur_chat_uid;
    QWidget* _last_widget;
    void SetSelectChatPage(int uid = 0);
    void SetSelectChatItem(int uid=0);
    void loadMoreChatUser();
    void loadMoreConUser();
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>>msgchat);
    void updatahead();
private slots:
    void slot_loading_chat_user();
    void solt_side_chat();
    void slot_side_contact();
    void slot_side_ai();
    void slot_text_changed(const QString&str);
    void slot_loading_contact_user();
public slots:
    void slot_apply_friend(std::shared_ptr<AddlyFriendApply> apply);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo>auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>auth_rsp);
    void slot_jump_chat_item(std::shared_ptr<SearchInfo>si);
    void slot_friendinfo_pge(std::shared_ptr<UserInfo> user_info);
    void slot_switch_apply_friend_page();
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info);
    void slot_item_clicked(QListWidgetItem*item);
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg>msg);
    void slot_change_head();
};

#endif // CHATDIALOG_HXX
