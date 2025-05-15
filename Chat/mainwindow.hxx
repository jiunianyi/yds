#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX
#include"logindialog.hxx"
#include"registerdialog.hxx"
#include <QMainWindow>
#include"resetdialog.hxx";
#include "chatdialog.hxx"
/******************************************************************************
 *
 * @file       mainwindow.hxx
 * @brief      主窗口
 *
 * @author     YDS
 * @date       2024/07/22
 * @history
 *****************************************************************************/
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void SlotSwitchReg();
    void SlotSwitchLogin();
    void SlotSwitchReset();
    void SlotSwitchLogin2();
    void SlotSwitchChat();
private:
    Ui::MainWindow *ui;
    loginDialog * _login_dlg;
    regiSterDialog * _reg_dlg;
    ResetDialog * _reset_dlg;
    chatDialog * _chat_dlg;
};
#endif // MAINWINDOW_HXX
