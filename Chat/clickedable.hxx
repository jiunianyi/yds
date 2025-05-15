#ifndef CLICKEDABLE_HXX
#define CLICKEDABLE_HXX

#include <QLabel>
#include<QMouseEvent>
//点击一次
class Clickedable : public QLabel
{
    Q_OBJECT
public:
    Clickedable(QWidget *parent=nullptr);
    virtual void mouseReleaseEvent(QMouseEvent *ev)override;
signals:
    void clicked(QString);
};

#endif // CLICKEDABLE_HXX
