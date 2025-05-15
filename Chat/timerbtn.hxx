#ifndef TIMERBTN_HXX
#define TIMERBTN_HXX

#include <QPushButton>
#include<QTimer>
class TimerBtn:public QPushButton
{
public:
    ~TimerBtn();
    TimerBtn(QWidget *parent = nullptr);
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QTimer  *_timer;
    int _counter;
};

#endif // TIMERBTN_HXX
