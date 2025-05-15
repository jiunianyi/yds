#ifndef FILEBUBBLE_HXX
#define FILEBUBBLE_HXX
#include"bubbleframe.hxx"
#include "qpushbutton.h"
#include<QHBoxLayout>
#include<QPixmap>

class fileBubble:public BubbleFrame
{
    Q_OBJECT
public:
    fileBubble(const QPixmap &picture,QPushButton *btn,ChatRole role,QWidget *parent=nullptr);
};

#endif // FILEBUBBLE_HXX
