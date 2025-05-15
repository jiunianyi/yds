#include "registerdialog.hxx"
#include "qtimer.h"
#include "ui_registerdialog.h"
#include<global.h>
#include<httpmgr.hxx>
#include<QMap>
#include<QDebug>
#include <QRandomGenerator>
#include <QRegularExpressionValidator>
regiSterDialog::regiSterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::regiSterDialog),_conutdown(5)
{
    ui->setupUi(this);
    ui->pwd_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    connect(HttpMgr::getinstance().get(),&HttpMgr::sig_reg_mod_finish,this,&regiSterDialog::slot_reg_mod_finish);

    inithttphandlers();
    QAction  *toggleAction = new QAction("显示", this);
    //toggleButton->setFixedSize(60, 25); // 设置按钮大小
    toggleAction->setIcon(QIcon(":/res/unvisible.png"));
    ui->pwd_edit->addAction(toggleAction, QLineEdit::TrailingPosition);
    ui->pwd_edit->setEchoMode(QLineEdit::Password);
    connect(toggleAction, &QAction::triggered, [this,toggleAction]() {
        // 切换回显模式
        if (ui->pwd_edit->echoMode() == QLineEdit::Password) {
            qDebug()<<"显示密码";
            ui->pwd_edit->setEchoMode(QLineEdit::Normal); // 显示密码
            toggleAction->setIcon(QIcon(":/res/visible_hover.png"));
        } else {
            qDebug()<<"隐藏密码";
            ui->pwd_edit->setEchoMode(QLineEdit::Password); // 隐藏密码
            toggleAction->setIcon(QIcon(":/res/unvisible.png"));
        }
    });
    QAction  *togg = new QAction("显示", this);
    //toggleButton->setFixedSize(60, 25); // 设置按钮大小
    togg->setIcon(QIcon(":/res/unvisible.png"));
    ui->confirm_edit->addAction(togg, QLineEdit::TrailingPosition);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    connect(togg, &QAction::triggered, [this,togg]() {
        // 切换回显模式
        if (ui->confirm_edit->echoMode() == QLineEdit::Password) {
            qDebug()<<"显示密码";
            ui->confirm_edit->setEchoMode(QLineEdit::Normal); // 显示密码
            togg->setIcon(QIcon(":/res/visible_hover.png"));
        } else {
            qDebug()<<"隐藏密码";
            ui->confirm_edit->setEchoMode(QLineEdit::Password); // 隐藏密码
            togg->setIcon(QIcon(":/res/unvisible.png"));
        }
    });
    ui->err_tip->clear();

    connect(ui->user_edit,&QLineEdit::editingFinished,this,[this](){
        checkUserValid();
    });
    connect(ui->email_edit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->pwd_edit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, [this](){
        checkConfirmValid();
    });
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, [this](){
        checkVarifyValid();
    });


    _countdown_timer = new QTimer(this);
    //连接页面切换的信号和曹
    connect(_countdown_timer,&QTimer::timeout,[this](){
        if(_conutdown==0)
        {
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _conutdown--;
        auto str = QString("注册成功，%1 s后返回登录").arg(_conutdown);
        ui->tip_lb->setText(str);
    });
}

regiSterDialog::~regiSterDialog()
{
    qDebug()<<"registerdialog is destruct";
    delete ui;
}

void regiSterDialog::on_pushButton_clicked()
{
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match)
    {
        //发送验证码
        QJsonObject json_obj;
        json_obj["email"]=email;
        HttpMgr::getinstance()->posthttpreq(QUrl(gate_url_prefix+"/get_varifycode"),
         json_obj,RedId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);

    }
    else
    {
        showtip(tr("邮箱地址不正确"),false);
    }
}

void regiSterDialog::slot_reg_mod_finish(RedId id, QString res, ErrorCodes err)
{
    if(err!=ErrorCodes::SUCCESS)
    {
        showtip(tr("网络请求失败"),false);
        return;
    }
    //解析json 字符串
    QJsonDocument jsondoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsondoc.isNull())
    {
        showtip(tr("json解析失败"),false);
        return;
    }
    //json解析错误
    if(!jsondoc.isObject())
    {
        showtip(tr("json解析失败"),false);
        return;
    }
    _handlers[id](jsondoc.object());
    return;
}

void regiSterDialog::inithttphandlers()
{
    //注册获取验证码回包
    _handlers.insert(RedId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonobj){
        int error =jsonobj["error"].toInt();
        if(error!=ErrorCodes::SUCCESS)
        {
            showtip(tr("参数错误"),false);
            return;
        }
        auto email = jsonobj["email"].toString();
        showtip(tr("验证码已经发送到邮箱，注意查收"),true);
        qDebug()<<"email is "<<email;
    });

    //注册注册用户回包逻辑
    _handlers.insert(RedId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
     if(error != ErrorCodes::SUCCESS){
            showtip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showtip(tr("用户注册成功"), true);
        qDebug()<<"user uuid is"<<jsonObj["uuid"].toString();
        qDebug()<< "email is " << email ;
        changeTipPage();
    });
}

void regiSterDialog::showtip(QString str,bool b_ok)
{
    if(b_ok)
    {
         ui->err_tip->setProperty("state","normal");
    }
    else
    {
          ui->err_tip->setProperty("state","err");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}

void regiSterDialog::changeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    //启动定时器
    _countdown_timer->start(1000);
}
void regiSterDialog::AddTipErr(TipErr te,QString tips)
{
    _tip_errs[te] = tips;
    showtip(tips, false);
}
void regiSterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showtip(_tip_errs.first(), false);
}
bool regiSterDialog::checkUserValid()
{
    if(ui->user_edit->text()=="")
    {
        AddTipErr(TipErr::TIP_USER_ERR,tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool regiSterDialog::checkEmailValid()
{
    //验证邮箱的地址正则表达式
    auto email = ui->email_edit->text();
    // 邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch(); // 执行正则表达式匹配
    if(!match){
        //提示邮箱不正确
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool regiSterDialog::checkPassValid()
{
    auto pass = ui->pwd_edit->text();
    if(pass.length() < 6 || pass.length()>15){
        //提示长度不准确
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应为6~15"));
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool regiSterDialog::checkConfirmValid()
{
    auto pwd =ui->confirm_edit->text();
    auto compwd = ui->pwd_edit->text();
    if(pwd!=compwd)
    {
        AddTipErr(TipErr::TIP_PWD_CONFIRM,tr("密码不匹配"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_CONFIRM);
    return true;
}

bool regiSterDialog::checkVarifyValid()
{

    auto pass = ui->varify_edit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}


void regiSterDialog::on_sure_btn_clicked()
{
    if(ui->user_edit->text() == ""){
        showtip(tr("用户名不能为空"), false);
        return;
    }
    if(ui->email_edit->text() == ""){
        showtip(tr("邮箱不能为空"), false);
        return;
    }
    if(ui->pwd_edit->text() == ""){
        showtip(tr("密码不能为空"), false);
        return;
    }
    if(ui->confirm_edit->text() == ""){
        showtip(tr("确认密码不能为空"), false);
        return;
    }
    if(ui->confirm_edit->text() != ui->pwd_edit->text()){
        showtip(tr("密码和确认密码不匹配"), false);
        return;
    }
    if(ui->varify_edit->text() == ""){
        showtip(tr("验证码不能为空"), false);
        return;
    }
    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = xorString(ui->pwd_edit->text());
    json_obj["confirm"] = xorString(ui->confirm_edit->text());
    json_obj["varifycode"] = ui->varify_edit->text();
    int randomValue = QRandomGenerator::global()->bounded(100); // 鐢熸垚0鍒?9涔嬮棿鐨勯殢鏈烘暣鏁?
    int head_i = randomValue % heads.size();
    json_obj["icon"] = heads[head_i];
    json_obj["nick"] = ui->user_edit->text();
    json_obj["sex"] = 0;
    HttpMgr::getinstance()->posthttpreq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, RedId::ID_REG_USER,Modules::REGISTERMOD);
}


void regiSterDialog::on_pushButton_2_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}


void regiSterDialog::on_cancle_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}




