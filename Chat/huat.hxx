#ifndef HUAT_HXX
#define HUAT_HXX

#include <QDialog>

namespace Ui {
class HuaT;
}

class HuaT : public QDialog
{
    Q_OBJECT

public:
    static HuaT& getInstance() {
        static HuaT instance; // 局部静态变量
        return instance;
    }
    explicit HuaT(QWidget *parent = nullptr);
    ~HuaT();

private slots:
    void stop();
signals:
    void guaduan();
    void sendgd();
private:
    Ui::HuaT *ui;
};

#endif // HUAT_HXX
