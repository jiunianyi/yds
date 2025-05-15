#include "clickedlabel.hxx"
#include<QMouseEvent>
#include<QEvent>

ClickedLabel::ClickedLabel(QWidget *parent):QLabel(parent),_curstate(ClickLbState::Normal)
{
this->setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton)
    {
        if(_curstate==ClickLbState::Normal)
        {
            qDebug()<<"clicked,change to selected hover:"<<_selected_hover;
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected_press);
            repolish(this);
            update();
        }else
        {
            qDebug()<<"clicked,change to normal hover: "<<_normal_hover;
            _curstate=ClickLbState::Normal;
            setProperty("state",_normal_press);
            repolish(this);
            update();
        }
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(_curstate == ClickLbState::Normal){
            // qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",_normal_hover);
            repolish(this);
            update();

        }else{
            //  qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",_selected_hover);
            repolish(this);
            update();
        }
        emit clicked(this->text(), _curstate);
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(_curstate == ClickLbState::Normal){
        qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",_normal_hover);
        repolish(this);
        update();
    }else{
        qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",_selected_hover);
        repolish(this);
        update();
    }
    //QLabel::enterEvent(event);
}
void ClickedLabel::leaveEvent(QEvent *event)
{
    if(_curstate == ClickLbState::Normal){
        qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",_normal);
        repolish(this);
        update();
    }else{
        qDebug()<<"leave , change to normal hover: "<< _selected;
        setProperty("state",_selected);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;//无光
    _normal_hover = hover;//无光显示
    _normal_press = press;

    _selected = select;//有光
    _selected_hover = select_hover;//有光显示
    _selected_press = select_press;
    setProperty("state",normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState()
{
    return _curstate;
}

bool ClickedLabel::SetCurState(ClickLbState state)
{
    _curstate = state;
    if(_curstate==ClickLbState::Normal)
    {
        setProperty("state",_normal);
        repolish(this);
    }
    else if(_curstate==ClickLbState::Selected)
    {
        setProperty("state",_selected);
        repolish(this);
    }
    return true;
}

void ClickedLabel::ResetNormalState()
{
    _curstate=ClickLbState::Normal;
    setProperty("state",_normal);
    repolish(this);
}
