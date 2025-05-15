#include "ai.hxx"
#include "qjsonarray.h"
#include "qjsondocument.h"
#include "qjsonobject.h"
#include "qpushbutton.h"
#include "qscrollbar.h"
#include "qsettings.h"
#include "qtimer.h"
#include "ui_ai.h"
#include<QMenu>
#include<QMessageBox>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QFormLayout>
#include"global.h"
AI::AI(
    QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AI)
{
    ui->setupUi(this);
    m_networkManager=new QNetworkAccessManager(this);
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // 指定配置文件的路径
    QString configFile = "config.ini";

    // 创建 QSettings 对象，指定文件路径和 INI 格式
    QSettings settings(configFile, QSettings::IniFormat);

    // 读取 [deepseek] 组下的 key 和 Url
    m_apiKey = settings.value("deepseek/key", "").toString();
    m_apiUrl = settings.value("deepseek/Url", "").toString();

    setupMenu(mainLayout);
    setaichatk(mainLayout);
    setapipage(mainLayout);
    connect(m_sendButton, &QPushButton::clicked, this, &AI::sendMessage);
    connect(m_inputLine, &QLineEdit::returnPressed, this, &AI::sendMessage);
}

AI::~AI()
{
    delete ui;
}

void AI::setupMenu(QVBoxLayout *mainLayout)
{    // 文件菜单
    // 创建一个新的菜单栏并设置父对象为当前对话
    QMenuBar *menuBar = new QMenuBar(this);

    // 文件菜单
    QMenu *fileMenu = menuBar->addMenu("文件(&F)");

    QAction *saveAction = new QAction("保存对话", this);
    saveAction->setShortcut(QKeySequence::Save);
    // 连接信号和槽
   // connect(saveAction, &QAction::triggered, this, &AI::saveConversation);
    fileMenu->addAction(saveAction);

    QAction *clearAction = new QAction("清空对话", this);
  //  connect(clearAction, &QAction::triggered, this, &AI::clearConversation);
    fileMenu->addAction(clearAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
   // connect(exitAction, &QAction::triggered, this, &QDialog::close);
    fileMenu->addAction(exitAction);

    // 设置菜单
    QMenu *settingsMenu = menuBar->addMenu("设置(&S)");

    QAction *apiSettingsAction = new QAction("API设置", this);
    connect(apiSettingsAction, &QAction::triggered, this, &AI::showSettingsDialog);
    settingsMenu->addAction(apiSettingsAction);

    // 帮助菜单
    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");

    QAction *aboutAction = new QAction("关于", this);
    connect(aboutAction, &QAction::triggered, this, &AI::about);
    helpMenu->addAction(aboutAction);

    // 将菜单栏添加到主布局
    mainLayout->addWidget(menuBar);

}

void AI::setaichatk(QVBoxLayout *mainLayout)
{
    m_chatDisplay = new QTextEdit(this);
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_chatDisplay->setObjectName("chatDisplay");

    m_inputLine = new QLineEdit(this);
    m_inputLine->setPlaceholderText("输入消息...");
    m_inputLine->setObjectName("inputLine");

    m_sendButton = new QPushButton("发送", this);
    m_sendButton->setDefault(true);
    m_sendButton->setObjectName("sendButton");

    m_clearButton = new QPushButton("清空", this);
    m_clearButton->setObjectName("clearButton");

    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_statusLabel->setObjectName("statusLabel");

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_inputLine, 1);
    inputLayout->addWidget(m_sendButton);
    inputLayout->addWidget(m_clearButton);

    QVBoxLayout *mainLa = new QVBoxLayout();
    mainLa->addWidget(m_chatDisplay, 1);
    mainLa->addLayout(inputLayout);
    mainLa->addWidget(m_statusLabel);

    // 将聊天布局添加到主布局
    mainLayout->addLayout(mainLa);
}

void AI::setapipage(QVBoxLayout* mainLayout)
{
    m_settingsPanel = new QWidget(this);
    m_settingsPanel->setVisible(false); // 初始隐藏
    // 设置面板固定高度为100像素
    m_settingsPanel->setFixedHeight(50);
    m_settingsPanel->setFixedWidth(300);
    m_settingsPanel->setMinimumWidth(300);
    // 设置面板的最小高度和最大高度
    m_settingsPanel->setMinimumHeight(100);
    m_settingsPanel->setMaximumHeight(2000);
    m_settingsPanel->setGeometry(50, 50, 200, 150);

    // 设置面板布局
    QVBoxLayout* settingsLayout = new QVBoxLayout(m_settingsPanel);
    QFormLayout* formLayout = new QFormLayout();

    m_settingsApiKeyLineEdit = new QLineEdit();
    m_settingsApiKeyLineEdit->setEchoMode(QLineEdit::Password);
    m_settingsApiUrlLineEdit = new QLineEdit();
     m_settingsApiUrlLineEdit->setEchoMode(QLineEdit::Password);
    m_settingsApiKeyLineEdit->setText(m_apiKey);
    m_settingsApiUrlLineEdit->setText(m_apiUrl);

    formLayout->addRow("API 密钥:", m_settingsApiKeyLineEdit);
    formLayout->addRow("API URL:", m_settingsApiUrlLineEdit);
    settingsLayout->addLayout(formLayout);

    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_settingsConfirmButton = new QPushButton("确认");
    m_settingsCancelButton = new QPushButton("取消");
    buttonLayout->addWidget(m_settingsConfirmButton);
    buttonLayout->addWidget(m_settingsCancelButton);
    settingsLayout->addLayout(buttonLayout);
    m_settingsConfirmButton->setObjectName("m_settingsConfirmButton");
    m_settingsCancelButton->setObjectName("m_settingsCancelButton");


    // 将设置面板添加到主布局
    //mainLayout->addWidget(m_settingsPanel);

    // 连接设置按钮信号
    connect(m_settingsConfirmButton, &QPushButton::clicked, this, &AI::confirmSettings);
    connect(m_settingsCancelButton, &QPushButton::clicked, this, &AI::cancelSettings);
}

QJsonObject AI::createPayload(const QString &message) const
{
      QJsonArray messagesArray;
    // 添加新消息
    QJsonObject newMsg;
    newMsg["role"] = "user";
    newMsg["content"] = message;
    messagesArray.append(newMsg);

    QJsonObject payload;
    payload["model"] = "deepseek-chat";
    payload["messages"] = messagesArray;
    payload["temperature"] = 0.7;
    payload["max_tokens"] = 2000;

    return payload;
}

void AI::confirmSettings()
{
    // 获取用户输入
    m_apiKey = m_settingsApiKeyLineEdit->text();
    m_apiUrl = m_settingsApiUrlLineEdit->text();
}

void AI::cancelSettings()
{
    // 隐藏设置面板
    m_settingsPanel->setVisible(false);
}

void AI::showStatusMessage(const QString &message, int timeout)
{
    m_statusLabel->setText(message);
    if (timeout > 0) {
        QTimer::singleShot(timeout, [this]() {
            m_statusLabel->setText("就绪");
        });
    }
}

void AI::addMessage(const ChatMessage &message)
{
    // 更新显示
    QPixmap pix("/res/ai.png");
    QLabel lb;
    lb.setPixmap(pix);
    QString prefix;
    // 根据消息角色选择图像
    QString imgPath;
    if (message.role == MessageRole::User) {
        imgPath = ":/res/deepseek.png"; // 确保资源路径正确
        prefix = ": ";
    } else {
        imgPath = ":/res/ai.png"; // 确保资源路径正确
        prefix = ": ";
    }
    QString formattedTime = message.timestamp.toString("hh:mm:ss");
   // QString displayText = QString("[%1] %2%3").arg(formattedTime, prefix, message.content);
    // 构建 HTML 字符串，嵌入图像和文本前缀
    QString displayText = QString("<p><img src='%2' width='32' height='32' alt='icon'> <b>%3</b> %4</p>")
                              .arg(imgPath, prefix.left(prefix.length() - 2), message.content);
    m_chatDisplay->append(displayText);
    m_chatDisplay->verticalScrollBar()->setValue(m_chatDisplay->verticalScrollBar()->maximum());
}

void AI::saveConversation()
{

}

void AI::clearConversation()
{

}

void AI::showSettingsDialog()
{
    // 切换设置面板的可见性
    bool isVisible = m_settingsPanel->isVisible();
    m_settingsPanel->setVisible(!isVisible);
}

void AI::about()
{
    QMessageBox::about(this, "不会聊天吗",
                       "<h2>DeepSeek Chat 1.0</h2>"
                       "<p>基于DeepSeek的Qt</p>"
                       "<p>Copyright © 2023 YourCompany</p>");
}

void AI::sendMessage()
{
    QString message = m_inputLine->text().trimmed();
    if (message.isEmpty()) {
        showStatusMessage("消息不能为空", 2000);
        return;
    }

    if (m_apiKey.isEmpty()) {
        showStatusMessage("错误: 未设置API密钥", 3000);
        //emit errorOccurred("API密钥未设置");
        return;
    }
    // 添加用户消息到历史记录和显示
    ChatMessage userMsg(MessageRole::User, message);
    addMessage(userMsg);
    m_inputLine->clear();
    showStatusMessage("发送消息中...");

    QNetworkRequest request((QUrl(m_apiUrl)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QJsonObject payload = createPayload(message);
    QByteArray data = QJsonDocument(payload).toJson();

    // 发送请求
    QNetworkReply *reply = m_networkManager->post(request, data);

    // 设置超时
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    timer->start(60000); // 30秒超时

    connect(timer, &QTimer::timeout, [this, reply]() {
        if (reply && reply->isRunning()) {
            reply->abort();
            showStatusMessage("请求超时", 6000);
        }
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, timer]() {
        timer->stop();
        timer->deleteLater();
        handleResponse(reply);
    });

}

void AI::handleResponse(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        QString error = QString("网络错误: %1").arg(reply->errorString());
        showStatusMessage(error, 5000);
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        QString error = "解析响应失败: " + parseError.errorString();
        showStatusMessage(error, 5000);
        return;
    }

    QJsonObject responseObj = doc.object();
    if (!responseObj.contains("choices") || !responseObj["choices"].isArray() ||
        responseObj["choices"].toArray().isEmpty()) {
        QString error = "无效的API响应格式";
        showStatusMessage(error, 5000);

        return;
    }

    QJsonObject choice = responseObj["choices"].toArray().first().toObject();
    if (!choice.contains("message") || !choice["message"].isObject()) {
        QString error = "无效的消息格式";
        showStatusMessage(error, 5000);

        return;
    }

    QJsonObject messageObj = choice["message"].toObject();
    QString content = messageObj["content"].toString();

    // 添加AI回复到记录和显示
    ChatMessage aiMsg(MessageRole::AI, content);
    addMessage(aiMsg);

    showStatusMessage("消息已接收", 2000);
}
