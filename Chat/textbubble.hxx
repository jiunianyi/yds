#ifndef TEXTBUBBLE_HXX
#define TEXTBUBBLE_HXX
#include"global.h"
#include"bubbleframe.hxx"
#include <QTextEdit>
#include<QHBoxLayout>

class TextBubble:public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);
protected:
    bool eventFilter(QObject *o,QEvent * e);
private:
    void adjustTextHeight();
    void setPlainText(const QString &text);
    void initStyleSheet();
    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_HXX
