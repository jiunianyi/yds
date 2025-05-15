#include "mainwindow.hxx"
#include "qmessagebox.h"
#include <QApplication>
#include<QDebug>
#include<QFile>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
     a.setWindowIcon(QIcon(":/icon.ico")); // 设置应用程序图标
    QFile qss(":/style/ydsstyle.qss");
    if(qss.open(QFile::ReadOnly))
    {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }else
    {//47.101.36.202
        qDebug("open flase");
    }
    QString filename = "config.ini";
    QString app_path=QCoreApplication::applicationDirPath();
    qDebug()<<app_path<<"测试";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + filename);
    QSettings settings(config_path,QSettings::IniFormat);//获得完成路径
    QString gate_host = settings.value("Gateserver/host").toString();
     QString gate_port = settings.value("Gateserver/port").toString();
     qDebug()<< gate_host << gate_port;
     QString ab = "47.101.36.202";
    gate_url_prefix = "http://"+ (QString)ab + ":" + "8080";

          // 显示消息框并等待用户点击
    MainWindow w;
    qDebug()<<gate_url_prefix;
    w.show();
    return a.exec();
}
