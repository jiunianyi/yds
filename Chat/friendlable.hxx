#ifndef FRIENDLABLE_HXX
#define FRIENDLABLE_HXX

#include <QFrame>

namespace Ui {
class Friendlable;
}

class Friendlable : public QFrame
{
    Q_OBJECT

public:
    explicit Friendlable(QWidget *parent = nullptr);
    ~Friendlable();
    void SetText(QString text);
    int Width();
    int Height();
    QString Text();
private:
    Ui::Friendlable *ui;
    QString _text;
    int _width;
    int _height;
public slots:
    void slot_close();
signals:
    void sig_close(QString);
};

#endif // FRIENDLABLE_HXX
