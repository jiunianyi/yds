#ifndef REGISTERDIALOG_HXX
#define REGISTERDIALOG_HXX

#include <QDialog>
#include"singleton.h"
namespace Ui {
class regiSterDialog;
}

class regiSterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit regiSterDialog(QWidget *parent = nullptr);
    ~regiSterDialog();

private slots:
    void on_pushButton_clicked();
    void slot_reg_mod_finish(RedId id,QString res,ErrorCodes err);

    void on_sure_btn_clicked();

    void on_pushButton_2_clicked();



    void on_cancle_btn_clicked();


private:
    void inithttphandlers();
    void showtip(QString str,bool b_ok);
    void changeTipPage();
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();
   void AddTipErr(TipErr te,QString tips);//标语的添加
    void DelTipErr(TipErr te);//标语的删除z
    Ui::regiSterDialog *ui;
    QMap<RedId,std::function<void(const QJsonObject&)> > _handlers;
    QMap<TipErr, QString> _tip_errs;

    QTimer * _countdown_timer;//计时器
    int _conutdown;//数字

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_HXX
