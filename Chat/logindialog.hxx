#ifndef LOGINDIALOG_HXX
#define LOGINDIALOG_HXX

#include <QDialog>
#include"global.h"

namespace Ui {
class loginDialog;
}

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = nullptr);
    ~loginDialog();

private:
    void initHttpHandlers();
    void initHead();
    bool checkUserValid();
    bool checkPwdValid();
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    QMap<TipErr ,QString > _tip_errs;
    void showtip(QString str,bool b_ok);
    bool enableBtn(bool enabled);
    Ui::loginDialog *ui;
    QMap<RedId,std::function<void(const QJsonObject&)> >_handlers;
    int _uid;
    QString _token;
public slots:
    void slot_forget_pwd();

signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo);
    void sig_connect_udp(ServerInfo);
private slots:
    void on_login_btn_clicked();
    void slot_login_mod_finish(RedId id, QString res, ErrorCodes err);
    void slot_tcp_con_finish(bool bsuccess);
    void slot_login_failed(int);
};

#endif // LOGINDIALOG_HXX
