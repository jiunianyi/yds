#ifndef SPSELE_HXX
#define SPSELE_HXX

#include <QDialog>

namespace Ui {
class spsele;
}

class spsele : public QDialog
{
    Q_OBJECT

public:
    static spsele& getInstance() {
        static spsele instance; // 局部静态变量
        return instance;
    }
    explicit spsele(QWidget *parent = nullptr);
    ~spsele();
public slots:
    void swsps();
private slots:
    void on_pushButton_2_clicked();
signals:
    void accship();
private:
    Ui::spsele *ui;
};

#endif // SPSELE_HXX
