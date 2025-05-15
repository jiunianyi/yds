#ifndef CLICKEDBTN_HXX
#define CLICKEDBTN_HXX

#include <QPushButton>


class clickedbtn:public QPushButton
{
    Q_OBJECT
public:
    clickedbtn(QWidget * parent = nullptr);
    ~clickedbtn();
    void SetState(QString normal,QString hover, QString press);
protected:
    virtual void enterEvent(QEnterEvent *event)override;
    virtual void leaveEvent(QEvent *event)override;//鼠标离开
    virtual void mousePressEvent(QMouseEvent *event)override;//鼠标按下
    virtual void mouseReleaseEvent(QMouseEvent *event)override;//鼠标释放
private:
    QString _normal;
    QString _hover;
    QString _press;
};

#endif // CLICKEDBTN_HXX
