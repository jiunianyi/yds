#include "chatdialog.hxx"
#include "conuseritem.hxx"
#include"ui_chatdialog.h"
#include<QAction>
#include<QRandomGenerator>
#include<QDebug>
#include"chatuserwid.hxx"
#include"loadingdlg.hxx"
#include<QMouseEvent>
#include"global.h"
#include"tcpmgr.hxx"
#include"usermgr.h"
#include<QFileDialog>
#include"searchlist.hxx"

chatDialog::chatDialog(QWidget *parent)
    : QDialog(parent),ui(new Ui::chatDialog),_mode(ChatUIMode::ChatMode)
    , _state(ChatUIMode::ChatMode),_b_loading(false),_cur_chat_uid(0), _last_widget(nullptr)
{

    ui->setupUi(this);
    ui->add_btn->SetState("normal","hover","press");
    ui->search_edit->SetMaxLength(20);

    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->search_edit->addAction(searchAction,QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));

    //创建清楚动作并设置图标
    QAction *clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    // 初始时不显示清除图标
    // 将清除动作添加到LineEdit的末尾位置
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);

    // 当需要显示清除图标时，更改为实际的清除图标
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text) {
        if (!text.isEmpty()) {
            clearAction->setIcon(QIcon(":/res/close_search.png"));
        } else {
            clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 文本为空时，切换回透明图标
        }

    });

    // 连接清除动作的触发信号到槽函数，用于清除文本
    connect(clearAction, &QAction::triggered, [this, clearAction]() {
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/close_transparent.png")); // 清除文本后，切换回透明图标
        ui->search_edit->clearFocus();
        //清除按钮被按下则不显示搜索框
        ShowSerach(false);
    });
   ShowSerach(false);
    //连接加载更多信息的信号
   connect(ui->chat_user_list,&ChatUserList::sig_loading_chat_user,this,&chatDialog::slot_loading_chat_user);
    addChatUserList();
   solt_side_chat();
    QPixmap pix(":/res/AI.jpg");
   QPixmap pixmap(":/res/head_1.jpg");
   ui->side_head_lb->setPixmap(pixmap); // 将图片设置到QLabel上
   QPixmap scaledPixmap = pixmap.scaled( ui->side_head_lb->size(), Qt::KeepAspectRatio); // 将图片缩放到label的大小
   ui->side_head_lb->setPixmap(scaledPixmap); // 将缩放后的图片设置到QLabel上
   ui->side_head_lb->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
   updatahead();
   ui->side_chat_lb->setProperty("state","normal");
   ui->side_chat_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
   ui->side_contact_lb->SetState("normal","hover","pressed","selected_normal","selected_hover","selected_pressed");
   AddLBGroup(ui->AI_chat_lb);
   AddLBGroup(ui->side_chat_lb);
   AddLBGroup(ui->side_contact_lb);
   connect(ui->side_chat_lb, &StateWidget::clicked, this, &::chatDialog::solt_side_chat);
   connect(ui->side_contact_lb, &StateWidget::clicked, this, &chatDialog::slot_side_contact);
   connect(ui->AI_chat_lb,&StateWidget::clicked,this,&chatDialog::slot_side_ai);


   //链接搜索框输入变化
   connect(ui->search_edit,&QLineEdit::textChanged,this,&chatDialog::slot_text_changed);
   //检查鼠标的点击位置判断是否要清空事件框
   this->installEventFilter(this);//安装事件过滤器
   //设置聊天框是选中状态
   ui->side_chat_lb->SetSelected(true);
   ui->serach_list->SetSearchEdit(ui->search_edit);

   //设置选中条目
   SetSelectChatItem();
   //更新聊天界面
   SetSelectChatPage();
   //点击添加
   //申请添加好友
   connect(TcpMgr::getinstance().get(),&TcpMgr::sig_friend_apply,this,&chatDialog::slot_apply_friend);
   //申请认证好友
   connect(TcpMgr::getinstance().get(),&TcpMgr::sig_add_auth_friend,this,&chatDialog::slot_add_auth_friend);
   //连接直接认证后的回报
   connect(TcpMgr::getinstance().get(),&TcpMgr::sig_auth_rsp,this,&::chatDialog::slot_auth_rsp);
   //连接serach跳转信号
   connect(ui->serach_list,&SearchList::sig_jump_chat_item,this,&chatDialog::slot_jump_chat_item);
   //连接加载联系人的信号和槽函数
   connect(ui->con_urse_list,&ContactUserList::sig_loading_contact_user,this,&chatDialog::slot_loading_contact_user);

   //链接点击联系人item发出信号和用户信息展示槽函数
   connect(ui->con_urse_list,&ContactUserList::sig_switch_friend_info_page,this,&chatDialog::slot_friendinfo_pge);
   //连接联系人页面点击好友申请条目的信号
   connect(ui->con_urse_list,&ContactUserList::sig_switch_apply_friend_page,this,&chatDialog::slot_switch_apply_friend_page);
   //连接好友信息界面的点击事件
   connect(ui->friend_info_page,&FriendInfoPage::sig_jump_chat_item,this,&chatDialog::slot_jump_chat_item_from_infopage);
   //连接聊天列表点击信号
   connect(ui->chat_user_list,&QListWidget::itemClicked,this,&chatDialog::slot_item_clicked);
   //
   connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this, &chatDialog::slot_append_send_chat_msg);
   //连接对端消息通知
   connect(TcpMgr::getinstance().get(),&TcpMgr::sig_text_chat_msg,this,&chatDialog::slot_text_chat_msg);
   //更换头像
   connect(ui->side_head_lb,&ClickedLabel::clicked,this,&chatDialog::slot_change_head);

}

chatDialog::~chatDialog()
{
    delete ui;
}
void chatDialog::addChatUserList()
{


    auto friend_list = UserMgr::getinstance()->GetChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter != _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;//为了在列表中嵌入小部件
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);//将自定义部件列表可视化
            _chat_items_added.insert(friend_ele->_uid, item);
        }
    // 创建QListWidgetItem，并设置自定义的widget
   /* for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();

        auto *chat_user_wid = new ChatUserWid();
        auto user_info = std::make_shared<UserInfo>(0,names[name_i],names[name_i],heads[head_i],0,strs[str_i]);
        chat_user_wid->SetInfo(user_info);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }*/
}
}

void chatDialog::ShowSerach(bool bsersch)
{
    if(bsersch){
        ui->chat_user_list->hide();
        ui->con_urse_list->hide();
        ui->serach_list->show();
        //qDebug()<<"asd";
        _mode = ChatUIMode::SerachMode;
    }else if(_state == ChatUIMode::ChatMode){
        qDebug()<<"asd";
        ui->chat_user_list->show();
        ui->con_urse_list->hide();
        ui->serach_list->hide();
        ui->side_contact_lb->ShowRedPoint(false);
        ui->con_urse_list->ShowRedPoint(false);
        _mode = ChatUIMode::ChatMode;
    }else if(_state == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->serach_list->hide();
        ui->con_urse_list->show();
        //ui->con_urse_list->ShowRedPoint(false);
       // ui->side_contact_lb->ShowRedPoint(false);
        _mode = ChatUIMode::ContactMode;
    }
}

void chatDialog::AddLBGroup(StateWidget *lb)
{
    _lb_list.push_back(lb);
}


void chatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto & ele: _lb_list){
        if(ele == lb){
            continue;
        }

        ele->ClearState();
    }
}

bool chatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type()==QEvent::MouseButtonPress)
    {
         QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }
    return true;
}

void chatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    // 实现点击位置的判断和处理逻辑
    // 先判断是否处于搜索模式，如果不处于搜索模式则直接返回
    if( _mode != ChatUIMode::SerachMode){
        return;
    }

    // 将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->serach_list->mapFromGlobal(event->globalPos());
    // 判断点击位置是否在聊天列表的范围内
    if (!ui->serach_list->rect().contains(posInSearchList)) {
        // 如果不在聊天列表内，清空输入框
        ui->search_edit->clear();
        ShowSerach(false);
    }
}
void chatDialog::slot_loading_chat_user()
{
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreChatUser();
    // 加载完成后关闭对话框
    loadingDialog->deleteLater();

    _b_loading = false;
}

void chatDialog::solt_side_chat()
{
    qDebug()<< "receive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSerach(false);
}

void chatDialog::slot_side_contact()
{
    qDebug()<< "receive side contzct clicked";
    ClearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    _state = ChatUIMode::ContactMode;
    ShowSerach(false);
}

void chatDialog::slot_side_ai()
{
    qDebug()<< "receive side ai clicked";
    ClearLabelState(ui->AI_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->AI_page);
    _state = ChatUIMode::AIMode;
    ShowSerach(false);
}

void chatDialog::slot_text_changed(const QString&str)
{
    if(!str.isEmpty())
    {
        ShowSerach(true);
    }
}

void chatDialog::slot_loading_contact_user()
{
    qDebug() << "slot loading contact user";
    if(_b_loading){
        return;
    }

    _b_loading = true;
    LoadingDlg *loadingDialog = new LoadingDlg(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list.....";
    loadMoreConUser();
//关闭
    loadingDialog->deleteLater();

    _b_loading = false;
}

void chatDialog::slot_apply_friend(std::shared_ptr<AddlyFriendApply> apply)
{
    qDebug()<<"receive apply firend slot,spplyuid is "<<apply->_from_uid<<"name is"<<apply->_name
             <<"desc is "<<apply->_desc;
    bool b_already = UserMgr::getinstance()->Alreadyapply(apply->_from_uid);
    if(b_already)
    {
        return;
    }
    UserMgr::getinstance()->Addapplylist(std::make_shared<ApplyInfo>(apply));
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_urse_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void chatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    qDebug() << "receive slot_add_auth__friend uid is " << auth_info->_uid
             << " name is " << auth_info->_name << " nick is " << auth_info->_nick;


    auto bfriend = UserMgr::getinstance()->CheckFriendById(auth_info->_uid);
    if(bfriend){
        return;
    }

    UserMgr::getinstance()->Addfriend(auth_info);


    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_info->_uid, item);
}

void chatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "receive slot_auth_rsp uid is " << auth_rsp->_uid
             << " name is " << auth_rsp->_name << " nick is " << auth_rsp->_nick;


    auto bfriend = UserMgr::getinstance()->CheckFriendById(auth_rsp->_uid);
    if(bfriend){
        return;
    }

    UserMgr::getinstance()->Addfriend(auth_rsp);
    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_rsp->_uid, item);
}

void chatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "slot jump chat item " << Qt::endl;
    auto find_iter = _chat_items_added.find(si->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << si->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(si->_uid);
        //更新聊天界面
        SetSelectChatPage(si->_uid);
        solt_side_chat();
        return;
    }

    //如果没有找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(si);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(si->_uid);
    //更新界面
    SetSelectChatPage(si->_uid);
    solt_side_chat();
}

void chatDialog::SetSelectChatItem(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }

    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem *firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }

        //
        QWidget *widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }

        _cur_chat_uid = con_item->GetUserinfo()->_uid;

        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        qDebug() << "uid " <<uid<< " not found, set curent row 0";
        ui->chat_user_list->setCurrentRow(0);
        return;
    }

    ui->chat_user_list->setCurrentItem(find_iter.value());

    _cur_chat_uid = uid;
}

void chatDialog::slot_friendinfo_pge(std::shared_ptr<UserInfo> user_info)
{
    qDebug()<<"receive switch firend info page sig";
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void chatDialog::slot_switch_apply_friend_page()
{
    qDebug()<<"receive switch apply friend page sig";
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void chatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "slot jump chat item " << Qt::endl;
    auto find_iter = _chat_items_added.find(user_info->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item , uid is " << user_info->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(user_info->_uid);
        //更新聊天界面
        SetSelectChatPage(user_info->_uid);
        solt_side_chat();
        return;
    }

    //如果没有找到，侧创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);

    _chat_items_added.insert(user_info->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(user_info->_uid);
    //更新聊天界面
    SetSelectChatPage(user_info->_uid);
    solt_side_chat();
}

void chatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item); //
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    //
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }


    if(itemType == ListItemType::CHAT_USER_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto user_info = chat_wid->GetUserinfo();
        //跳转到聊天界面
        ui->chat_page->SetUserInfo(user_info);
        _cur_chat_uid = user_info->_uid;
        return;
    }
}

void chatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if (_cur_chat_uid == 0) {
        return;
    }

    auto find_iter = _chat_items_added.find(_cur_chat_uid);
    if (find_iter == _chat_items_added.end()) {
        return;
    }

    //转为widget
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if (!widget) {
        return;
    }


    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if (itemType == CHAT_USER_ITEM) {
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if (!con_item) {
            return;
        }


        auto user_info = con_item->GetUserinfo();
        user_info->_chat_msgs.push_back(msgdata);
        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msgdata);
        UserMgr::getinstance()->AppendFriendChatMsg(_cur_chat_uid,msg_vec);
        return;
    }
}

void chatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    auto find_iter = _chat_items_added.find(msg->_from_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "set chat item msg, uid is " << msg->_from_uid;
        QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            return;
        }
        chat_wid->updateLastMsg(msg->_chat_msgs);
        //更新当前聊天页面记录
        UpdateChatMsg(msg->_chat_msgs);
        UserMgr::getinstance()->AppendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
        return;
    }

    //如果没找到，则创建新的插入listwidget

    auto* chat_user_wid = new ChatUserWid();
    //查询好友信息
    auto fi_ptr = UserMgr::getinstance()->GetfriendByid(msg->_from_uid);
    chat_user_wid->SetInfo(fi_ptr);
    QListWidgetItem* item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg->_chat_msgs);
    UserMgr::getinstance()->AppendFriendChatMsg(msg->_from_uid,msg->_chat_msgs);
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(msg->_from_uid, item);
}
//更换头像
void chatDialog::slot_change_head()
{
    QString path = QFileDialog::getOpenFileName(this,tr("选择头像"),QDir::homePath(),tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if(!path.isEmpty())
    {
        QPixmap pix(path);
        if(!pix.isNull())
        {
            QPixmap scaledPix = pix.scaled(
                ui->side_head_lb->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
              ui->side_head_lb->setPixmap(scaledPix);
            //保存图片到注册表（windows）根据系统来定
            QSettings settings("MyCompany", "ChatApp");
            settings.setValue("avatarPath", path);
        }
    }
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    user_info->_icon = path;

}


void chatDialog::SetSelectChatPage(int uid)
{
    if( ui->chat_user_list->count() <= 0){
        return;
    }

    if (uid == 0) {
        auto item = ui->chat_user_list->item(0);
        //转为widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if (!widget) {
            return;
        }

        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if (!con_item) {
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserinfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }

    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        return;
    }

    //转为widget
    QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }

    //判断转化为自定义的widget
    // 对自定义widget进行操作，将item转换为基类listitembase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "qobject_cast<ListItemBase*>(widget) is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }

        //设置信息
        auto user_info = con_item->GetUserinfo();
        ui->chat_page->SetUserInfo(user_info);

        return;
    }
}

void chatDialog::loadMoreChatUser()
{
    auto friend_list = UserMgr::getinstance()->GetChatListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter != _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }

        //更新已加载条目
        UserMgr::getinstance()->UpdateChatLoadedCount();
    }
}

void chatDialog::loadMoreConUser()
{
    auto friend_list = UserMgr::getinstance()->GetConListPerPage();
    if (friend_list.empty() == false) {
        for(auto & friend_ele : friend_list){
            auto *chat_user_wid = new ConUserItem();
            chat_user_wid->SetInfo(friend_ele->_uid,friend_ele->_name,
                                   friend_ele->_icon);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->con_urse_list->addItem(item);
            ui->con_urse_list->setItemWidget(item, chat_user_wid);
        }


        UserMgr::getinstance()->UpdateContactLoadedCount();
    }
}

void chatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgchat)
{
    for(auto &msg:msgchat)
    {
        if(msg->_from_uid!=_cur_chat_uid)
        {
            break;
        }
        ui->chat_page->AppendChatMsg(msg);
    }
}

void chatDialog::updatahead()
{
    QSettings settings("MyCompany", "ChatApp");

    // 从设置中加载头像路径
    QString savedPath = settings.value("avatarPath", "").toString();
    auto user_info = UserMgr::getinstance()->GetUserInfo();
    user_info->_icon = savedPath;
    if (!savedPath.isEmpty() && QFile::exists(savedPath)) {
        QPixmap pix(savedPath);
        if (!pix.isNull()) {
            // 将头像图片设置到 QLabel
            ui->side_head_lb->setPixmap(pix.scaled(
                ui->side_head_lb->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));
        }else {
            // 加载默认头像或显示占位符
            ui->side_head_lb->setText("头像");
        }
}else {
        // 如果没有保存的路径，设置默认头像或显示占位符
        ui->side_head_lb->setText("头像");
    }
}
