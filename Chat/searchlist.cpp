#include "searchlist.hxx"
#include "qjsonobject.h"
#include"tcpmgr.hxx"
#include"customizeedit.hxx"
#include"global.h"
#include"loadingdlg.hxx"
#include"adduseritem.hxx"
#include"findsuccessdia.hxx"
#include<QJsonDocument>
#include"findfaildlg.hxx";
#include"usermgr.h"


SearchList::SearchList(QWidget *parent):QListWidget(parent),_send_pending(false)
    ,_find_dlg(nullptr),_search_edit(nullptr)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::getinstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::CloseFindDlg()
{
    if(_find_dlg)
    {
        _find_dlg->hide();
        _find_dlg=nullptr;
    }
}

void SearchList::SetSearchEdit(QWidget *edit)
{
    _search_edit = edit;
}

void SearchList::waitPending(bool pending)
{
    if(pending)
    {
        _loadingDialog = new LoadingDlg(this);
        _loadingDialog->setModal(true);
        _loadingDialog->show();
        _send_pending = pending;
        qDebug()<<"正在查找";
    }
    else{
        _loadingDialog->hide();
        _loadingDialog->deleteLater();
        _send_pending = pending;
    }
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item_tmp->setSizeHint(QSize(250,10));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); //获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_SUER_TIP_ITEM){

        //todo ...
        if(_send_pending)
        {
            return;
        }
        if(!_search_edit)
        {
            return;
        }
       waitPending(true);

        auto serach_edit = dynamic_cast<CustomizeEdit*>(_search_edit);
        auto uid_str = serach_edit->text();
        QJsonObject jsonObj;
        jsonObj["uid"]=uid_str;
        QJsonDocument doc(jsonObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Indented);
        qDebug()<<"发送搜索请求";
        emit TcpMgr::getinstance()->sig_send_data(RedId::ID_SEARCH_USER_REQ,jsonData);

        return;
    }

    //清楚弹出框
    CloseFindDlg();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{
    waitPending(false);
    if(si==nullptr)
    {
        qDebug()<<"未找到该用户";
        _find_dlg = std::make_shared<FindFailDlg>(this);
    }
    else{
        //todo
        auto self_uid = UserMgr::getinstance()->GetUid();
        if(si->_uid==self_uid)
        {
            return;
        }
        //搜到已经是好友
        bool bexit = UserMgr::getinstance()->CheckFriendById(si->_uid);
        if(bexit)
        {
            //已经拥有该好友，跳转到指定聊天界面
            emit sig_jump_chat_item(si);
            return;
        }
        //未添加好友
        _find_dlg = std::make_shared<FindSuccessDia>(this);
        std::dynamic_pointer_cast<FindSuccessDia>(_find_dlg)->SetSearchInfo(si);
    }
    _find_dlg->show();
}
