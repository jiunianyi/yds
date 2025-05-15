#ifndef FINDSUCCESSDIA_HXX
#define FINDSUCCESSDIA_HXX
#include<memory>
#include"userdata.hxx"
#include <QDialog>

namespace Ui {
class FindSuccessDia;
}

class FindSuccessDia : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDia(QWidget *parent = nullptr);
    ~FindSuccessDia();
    void SetSearchInfo(std::shared_ptr<SearchInfo>si);
private:
    Ui::FindSuccessDia *ui;
    QWidget *_parent;
    std::shared_ptr<SearchInfo>_si;
private slots:

    void on_add_firend_btn_clicked();
};

#endif // FINDSUCCESSDIA_HXX
