#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <QNetworkProxy>
#include <QNetworkRequest>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QScreen>
#include <QFileDialog>
#include "mynetworkaccessmanagerfactory.h"
#include "systemtrayicon.h"
#include "utility.h"
#include "mywindow.h"
#include "qqstars.h"
#include "myimage.h"
#include "mysvgview.h"
#include "myshortcut.h"
#include "myhttprequest.h"
#include "mymessagebox.h"
#include "downloadimage.h"
#include "texteditplaygif.h"
#include <QWebView>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName ("QQStars");
    app.setApplicationVersion ("1.0.0");
    app.setOrganizationName ("雨后星辰");
    app.setApplicationDisplayName ("星辰QQ");
    
    QTranslator *translator = new QTranslator;
    translator->load (":/qt_zh_CN.qm");
    QApplication::installTranslator (translator);
    
    QQmlApplicationEngine *engine = new QQmlApplicationEngine;
    engine->setNetworkAccessManagerFactory (new MyNetworkAccessManagerFactory());//给qml设置网络请求所用的类
    
    qmlRegisterType<TextEditPlayGif>("MyTextEditPlugin", 1, 0, "TextEditPlayGif");
    qmlRegisterType<MyWindow>("mywindow", 1,0, "MyQuickWindow");
    qmlRegisterType<SystemTrayIcon>("mywindow", 1,0, "MySystemTrayIcon");
    qmlRegisterType<MyMenu>("mywindow", 1,0, "MyMenu");
    qmlRegisterType<MenuSeparator>("mywindow", 1,0, "MenuSeparator");
    qmlRegisterType<MyMenuItem>("mywindow", 1,0, "MyMenuItem");
    qmlRegisterType<MyShortcut>("utility", 1,0, "MyShortcut");
    qmlRegisterType<DownloadImage>("utility", 1, 0, "DownloadImage");
    qmlRegisterType<QQCommand>("qqstars", 1,0, "QQ");
    qmlRegisterType<FriendInfo>("QQItemInfo", 1,0, "FriendInfo");
    qmlRegisterType<GroupInfo>("QQItemInfo", 1,0, "GroupInfo");
    qmlRegisterType<DiscuInfo>("QQItemInfo", 1,0, "DiscuInfo");
    qmlRegisterType<QQItemInfo>("QQItemInfo", 1,0, "QQItemInfo");
    qmlRegisterType<ChatMessageInfo>("QQItemInfo", 1, 0, "ChatMessageInfo");
    qmlRegisterType<ChatMessageInfoList>("QQItemInfo", 1, 0, "ChatMessageInfoList");
    qmlRegisterType<MyImage>("mywindow", 1,0, "MyImage");
    qmlRegisterType<MySvgView>("mywindow", 1, 0, "SvgView");
    qmlRegisterType<MyMessageBox>("mywindow", 1, 0, "MessageBox");
   
    Utility *utility=Utility::createUtilityClass ();
    QNetworkRequest* request = utility->getHttpRequest ()->getNetworkRequest ();
    request->setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");//和腾讯服务器打交道需要设置这个
    request->setHeader (QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    
    request = utility->getDownloadImage ()->getHttpRequest ()->getNetworkRequest ();
    request->setRawHeader ("Referer", "http://web2.qq.com/webqq.html");//需要设置这个，不然腾讯服务器不响应你的请求
    request->setRawHeader ("Accept", "image/webp,*/*;q=0.8");
    
    utility->initUtility (new QSettings, engine);
    
    QQmlComponent component0(engine, QUrl("qrc:/qml/Api/QQApi.qml"));
    QQCommand *qqapi = qobject_cast<QQCommand *>(component0.create ());

    engine->rootContext ()->setContextProperty ("myqq", qqapi);
    
    QQmlComponent component(engine, QUrl("qrc:/qml/Utility/SystemTray.qml"));
    SystemTrayIcon *systemTray = qobject_cast<SystemTrayIcon *>(component.create ());
#ifdef Q_OS_WIN
    systemTray->setParent (Utility::createUtilityClass ());//不设置父对象会导致程序退出后托盘还存在的问题
#endif
    engine->rootContext ()->setContextProperty ("systemTray", systemTray);//将程序托盘注册过去
    qqapi->loadLoginWindow ();//加载登录窗口
    
    return app.exec();
}
