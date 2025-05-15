#include "clickedbtn.hxx"
#include "global.h"


clickedbtn::clickedbtn(QWidget *parent):QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);//设置光标小手
    setFocusPolicy(Qt::NoFocus);
}
clickedbtn::~clickedbtn()
{

}

void clickedbtn::SetState(QString normal, QString hover, QString press)
{
    _hover = hover;
    _normal = normal;
    _press  = press;
    setProperty("state",_normal);
    repolish(this);
    update();
}

void clickedbtn::enterEvent(QEnterEvent *event)
{
    //qDebug()<<"dddd";
    setProperty("state",_hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void clickedbtn::leaveEvent(QEvent *event)
{
    setProperty("state",_normal);
    //qDebug()<<"dwsdef";
    repolish(this);
    update();
    QPushButton::leaveEvent(event);

}

void clickedbtn::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"鼠标进入";
    setProperty("state",_press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(event);
}

void clickedbtn::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"鼠标释放";
    setProperty("state",_hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(event);
}


