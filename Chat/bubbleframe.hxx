#ifndef BUBBLEFRAME_HXX
#define BUBBLEFRAME_HXX
#include<QFrame>
#include"global.h"
#include<QHBoxLayout>

class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget *parent = nullptr);
    void setMargin(int margin);

    void setWidget(QWidget *w);
protected:
    void paintEvent(QPaintEvent *e);
private:
    QHBoxLayout *m_pHLayout;//水平布局
    ChatRole m_role;
    int      m_margin;
};

#endif // BUBBLEFRAME_HXX
