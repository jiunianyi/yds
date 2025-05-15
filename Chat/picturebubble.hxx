#ifndef PICTUREBUBBLE_HXX
#define PICTUREBUBBLE_HXX
#include"bubbleframe.hxx"
#include<QHBoxLayout>
#include<QPixmap>

class PictureBubble:public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap &picture,ChatRole role,QWidget *parent=nullptr);
};

#endif // PICTUREBUBBLE_HXX
