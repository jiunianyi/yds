#ifndef ACCREF_HXX
#define ACCREF_HXX

#include <QDialog>

namespace Ui {
class accref;
}

class accref : public QDialog
{
    Q_OBJECT

public:
    static accref& getInstance() {
        static accref instance; // 局部静态变量
        return instance;
    }
    explicit accref(QWidget *parent = nullptr);
    ~accref();

public slots:
    void on_pushButton_2_clicked();
    void swad();
signals:
    void ref_yubyin();
    void accp_yuyin();
    void qi_start(int &id,int &touid);
private slots:
    void on_pushButton_clicked();

private:
    Ui::accref *ui;
};

#endif // ACCREF_HXX
