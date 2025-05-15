#include "customizeedit.hxx"

CustomizeEdit::CustomizeEdit(QWidget * parent):QLineEdit(parent),_max_len(0)
{
    connect(this,&QLineEdit::textChanged,this,&CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxlen)
{
    _max_len = maxlen;
}

