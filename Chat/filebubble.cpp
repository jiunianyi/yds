#include "filebubble.hxx"
#include "qlabel.h"



fileBubble::fileBubble(const QPixmap &picture, QPushButton *btn, ChatRole role, QWidget *parent)
:BubbleFrame(role,parent)
{
    QLabel *lb = new QLabel();
    lb->setPixmap(picture);
    lb->setScaledContents(true);
      QWidget *widget = new QWidget(this);
      QHBoxLayout *layout = new QHBoxLayout(widget);
      layout->addWidget(lb);
      btn->setStyleSheet("QPushButton { background-color: transparent; color: blue; text-decoration: underline; }");
      btn->setCursor(Qt::PointingHandCursor);
      layout->addWidget(btn);
      layout->addStretch();
      widget->setLayout(layout);
      this->setWidget(widget);

      int left_margin = this->layout()->contentsMargins().left();
      int right_margin = this->layout()->contentsMargins().right();
      int v_margin = this->layout()->contentsMargins().bottom();
      setFixedSize(picture.width()+left_margin+200 + right_margin, picture.height() + v_margin *2+15);

}
