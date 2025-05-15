#ifndef SELECTBTN_HXX
#define SELECTBTN_HXX

#include <QDialog>

namespace Ui {
class selectbtn;
}

class selectbtn : public QDialog
{
    Q_OBJECT

public:
    static selectbtn& getInstance() {
        static selectbtn instance; // 局部静态变量
        return instance;
    }
    explicit selectbtn(QWidget *parent = nullptr);
    ~selectbtn();
      Ui::selectbtn *ui;
    int touid=0;
      int fromuid = 0;
private slots:
    void on_pushButton_2_clicked();
    void on_cancel_btn_clicked();
public slots:
    void slot_close_sele();

signals:
    void sig_yuyin_req(QByteArray jsondata);

private:
    selectbtn(const selectbtn&) = delete; // 禁止拷贝构造
    selectbtn& operator=(const selectbtn&) = delete; // 禁止赋值

};

#endif // SELECTBTN_HXX
