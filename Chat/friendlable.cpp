#include "friendlable.hxx"
#include "ui_friendlable.h"

Friendlable::Friendlable(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::Friendlable)
{
    ui->setupUi(this);
    ui->close_lb->SetState("normlal","hover","pressed","selected_normal","selected_hover"
                           ,"selected_pressed");
    connect(ui->close_lb,&ClickedLabel::clicked,this,&Friendlable::slot_close);
}

Friendlable::~Friendlable()
{
    delete ui;
}

void Friendlable::SetText(QString text)
{
    _text=text;
    ui->tip_lb->setText(_text);
    ui->tip_lb->adjustSize();
    QFontMetrics fontMetrics(ui->tip_lb->font());
    auto textwidth = fontMetrics.horizontalAdvance(ui->tip_lb->text());//获得文本宽度
    auto textheight = fontMetrics.height();//获得文本高度

    this->setFixedHeight(textheight+2);
    this->setFixedWidth(ui->tip_lb->width()+ui->close_lb->width()+10);
    _width=this->width();
    _height=this->Height();
}

int Friendlable::Width()
{
    return _width;
}

int Friendlable::Height()
{
    return _height;
}

QString Friendlable::Text()
{
    return _text;
}

void Friendlable::slot_close()
{
    emit sig_close(_text);
}












