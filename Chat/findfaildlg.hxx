#ifndef FINDFAILDLG_HXX
#define FINDFAILDLG_HXX

#include <QDialog>

namespace Ui {
class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg();
private slots:
    void on_fail_sure_btn_clicked();
private:
    Ui::FindFailDlg *ui;
};

#endif // FINDFAILDLG_HXX
