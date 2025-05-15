#ifndef CHATITEMBASE_HXX
#define CHATITEMBASE_HXX
#include<QWidget>
#include<QLabel>
#include"global.h"
#include<QGridLayout>

class ChatItemBase:public QWidget
{
    Q_OBJECT
public:
   explicit ChatItemBase(ChatRole role,QWidget *parten = nullptr);
    void setUserName(const QString &name);
   void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *w);

private:
    ChatRole m_role;
    QLabel * m_pNameLabel;
    QLabel *m_pIconLable;
    QWidget *m_pBubble;

};

#endif // CHATITEMBASE_HXX
