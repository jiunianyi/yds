#include "mainwindow.hxx"
#include "ui_mainwindow.h"
#include "resetdialog.hxx"
#include "tcpmgr.hxx"
#include"udpmgr.hxx"

//#include"versionupgrades.hxx"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _login_dlg = new loginDialog(this);
    setCentralWidget(_login_dlg);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    _login_dlg->show();

    //创建和注册消息连接
    connect(_login_dlg,&loginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);
    //连接忘记密码界面
    connect(_login_dlg,&loginDialog::switchReset,this,&MainWindow::SlotSwitchReset);
    //连接聊天界面
    connect(TcpMgr::getinstance().get(),&TcpMgr::sig_switch_chatdlg,this,&MainWindow::SlotSwitchChat);
    //emit TcpMgr::getinstance()->sig_switch_chatdlg();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchReg()
{
    _reg_dlg = new regiSterDialog(this);
    _reg_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    //连接注册界面返回登录信号
    connect(_reg_dlg,&regiSterDialog::sigSwitchLogin,this,&MainWindow::SlotSwitchLogin);
    setCentralWidget(_reg_dlg);
    _login_dlg->hide();
    _reg_dlg->show();

}

void MainWindow::SlotSwitchLogin()
{
    //创建一个CentralWidget, 并将其设置为MainWindow的中心部件
    _login_dlg = new loginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);
    _reg_dlg->hide();
    _login_dlg->show();
    //连接登录界面注册信号
    connect(_login_dlg, &loginDialog::switchRegister, this, &MainWindow::SlotSwitchReg);
    //连接登录界面忘记密码信号
    connect(_login_dlg,&loginDialog::switchReset,this,&MainWindow::SlotSwitchReset);
}

void MainWindow::SlotSwitchReset()
{
    _reset_dlg = new ResetDialog(this);
    _reset_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_reset_dlg);
    _login_dlg->hide();
    _reset_dlg->show();
    //注册返回登录信号和槽函数
    connect(_reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);

}

void MainWindow::SlotSwitchLogin2()
{
    _login_dlg = new loginDialog(this);
    _login_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_login_dlg);

    _reset_dlg->hide();
    _login_dlg->show();
    //连接忘记密码界面
    connect(_login_dlg,&loginDialog::switchReset,this,&MainWindow::SlotSwitchReset);
    //连接登录界面注册信号
    connect(_login_dlg,&loginDialog::switchRegister,this,&MainWindow::SlotSwitchReg);

}

void MainWindow::SlotSwitchChat()
{
    _chat_dlg = new chatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    _login_dlg ->hide();
    this->setMinimumSize(QSize(1024,700));
    this->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
}








