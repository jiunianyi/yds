#ifndef AI_HXX
#define AI_HXX

#include "global.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qnetworkreply.h"
#include "qtextedit.h"
#include <QDialog>

namespace Ui {
class AI;
}

class AI : public QDialog
{
    Q_OBJECT

public:
    explicit AI(QWidget *parent = nullptr);
    ~AI();
   void setupMenu(QVBoxLayout *mainLayout);
   void setaichatk(QVBoxLayout *mainLayout);
   void setapipage(QVBoxLayout* mainLayout);
   QJsonObject createPayload(const QString &message) const;
   void confirmSettings();
   void cancelSettings();
   void showStatusMessage(const QString &message, int timeout = 3000);
   void addMessage(const ChatMessage &message);
public slots:
    void saveConversation();
    void clearConversation();
    void showSettingsDialog();
    void about();
    void sendMessage();
    void handleResponse(QNetworkReply *reply);
private:
    Ui::AI *ui;
    QTextEdit *m_chatDisplay;
    QLineEdit *m_inputLine;
    QPushButton *m_sendButton;
    QPushButton *m_clearButton;
    QLabel *m_statusLabel;

    QString m_apiKey;
    QString m_apiUrl;
      QNetworkAccessManager *m_networkManager;
    // 设置面板相关
    QWidget* m_settingsPanel;
    QLineEdit* m_settingsApiKeyLineEdit;
    QLineEdit* m_settingsApiUrlLineEdit;
    QPushButton* m_settingsConfirmButton;
    QPushButton* m_settingsCancelButton;
};

#endif // AI_HXX
