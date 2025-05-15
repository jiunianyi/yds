#ifndef GLOBAL_H
#define GLOBAL_H
#include<QWidget>
#include<functional>
#include<QRegularExpression>
#include<iostream>
#include"QStyle"
#include<memory>
#include<mutex>
#include<QByteArray>
#include<QNetworkReply>
#include<QMap>
#include<QDir>
#include<QSettings>

extern std::function<void(QWidget*)> repolish;//用来刷新提示
extern std::function<QString(QString)> xorString ;

struct k {
    QString filename;
    QString text;

    // 构造函数
    k(const QString &filePath, const QByteArray &fileData) {
        filename = QFileInfo(filePath).fileName();
        text = fileData.toBase64();
    }
};
const std::vector<QString>  strs ={"hello world !",
                             "nice to meet u",
                             "New year，new life",
                             "You have to love yourself",
                             "My love is written in the wind ever since the whole world is you"};

const std::vector<QString> heads = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg"
};

const std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};
enum RedId{
    //功能id
    ID_GET_VARIFY_CODE=1001,//获取验证码
    ID_REG_USER=1002,//注册用户
    ID_RESET_PWD = 1003,//重置密码
    ID_LOGIN_USER = 1004,//用户登录
    ID_CHAT_LOGIN = 1005,//登录聊天服务器
    ID_CHAT_LOGIN_RSP = 1006,//登录聊天服务器回包
    ID_SEARCH_USER_REQ = 1007,//用户搜索请求
    ID_SESRCH_USER_RSP = 1008,//搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,//添加好友申请
    ID_ADD_FRIEND_RSP = 1010,//申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,//通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,//认证好友申请
    ID_AUTH_FRIEND_RSP = 1014,//认证好友回复
    ID_NOTIF_AUTH_FRIEND_REQ = 1015,//通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,//文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,//文本聊天信息请求////文本聊天信息回复
     ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
    ID_SELECT_YUYIN_REQ = 1020,//进行语音通话请求
    ID_ACCEPT_YUNYI_REQ = 1021,//接受语音通话请求
    ID_REFUSE_YUYIN_REQ = 1022,//拒绝语音通话请求
    ID_SELECT_SHIPIN_REQ = 1023,//选择视频通话请求
    ID_ACCEPT_SHIPIN_REQ = 1024,//接受视频通话请求
    ID_REFUSE_SHIPIN_REQ = 1025,//拒绝视频通话请求
    ID_REGUSER_UDP = 1026,//注册udp用户
    ID_HEARD = 1027,//心跳
    ID_YUYIN = 1028,//语音
    ID_CHANG_HEAD = 1029,//更换头像
    ID_SHIPIN = 1030,//视频
    ID_YYGUADUAN = 1031,//挂断语音
    ID_SPGUADUAN = 1032//视频挂断
};
enum  MessageRole {
    User,
    AI
};
struct ChatMessage {
    QDateTime timestamp;
    MessageRole role;
    QString content;

    ChatMessage(MessageRole r = MessageRole::User, const QString &c = QString())
        : timestamp(QDateTime::currentDateTime()), role(r), content(c) {}
};
struct netwheard{
    int id;
    int uidd;
};

enum Modules{
    REGISTERMOD=0,
    RESETMOD=1,
    LOGINMOD=2,
};
enum ErrorCodes{
    SUCCESS=0,
    ERR_JSON=1,//json 解析失败
    ERR_NETWORK=2,//网络错误
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,//密码不匹配
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int uid;
    QString name;
};

//聊天的模式
enum ChatUIMode{
    SerachMode,//搜索模式
    ChatMode,//聊天模式
    ContactMode,//联系模式
    AIMode,//聊天模式
};
//自定义QListWidgetITem
enum ListItemType{
    CHAT_USER_ITEM,//聊天用户
    CONTACT_USER_ITEM,//联系人用户
    SERACH_USER_ITEM,//搜索到的用户
    ADD_SUER_TIP_ITEM,//提示添加用户
    INVALID_ITEM,//不可点击条目
    GROUP_TIP_ITEM,//分组提示条目
    LINE_ITEM,//分割线
    APPLY_FRIEND_ITEM//添加好友
};
enum class ChatRole{
    self,
    other,
};
struct Msginfo{
    QString msgFlage;//text,image,file
    QString content;//表示文件和图像的url，文本信息
    QPixmap pixmap;//文件和图片的缩略图
};
//申请好友标签输入框长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

const QString add_prefix = "添加标签: ";

const int tip_offset =5;


extern QString gate_url_prefix;

const int CHAT_COUNT_PER_PAGE = 13;

#endif // GLOBAL_H

