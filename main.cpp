#include <QApplication>
#include <QDebug>
#include <QNetworkProxy>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QScreen>
#include "systemtrayicon.h"
#include "utility.h"
#include "mywindow.h"
#include "qqstars.h"
#include "myimage.h"
#include "threaddownloadimage.h"
#include "mysvgview.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName ("QQStars");
    app.setApplicationVersion ("1.0.0");
    app.setOrganizationName ("雨后星辰");
    app.setApplicationDisplayName ("星辰QQ");
    
    QTranslator translator;
    translator.load (":/qt_zh_CN");
    QApplication::installTranslator (&translator);
    
    QQmlApplicationEngine engine;
    engine.setNetworkAccessManagerFactory (new MyNetworkAccessManagerFactory());//给qml设置网络请求所用的类
    
    qmlRegisterType<MyWindow>("mywindow", 1,0, "MyQuickWindow");
    qmlRegisterType<SystemTrayIcon>("mywindow", 1,0, "MySystemTrayIcon");
    qmlRegisterType<MyMenu>("mywindow", 1,0, "MyMenu");
    qmlRegisterType<MenuSeparator>("mywindow", 1,0, "MenuSeparator");
    qmlRegisterType<MyMenuItem>("mywindow", 1,0, "MyMenuItem");
    qmlRegisterType<QQCommand>("utility", 1,0, "QQ");
    qmlRegisterType<MyImage>("mywindow", 1,0, "MyImage");
    qmlRegisterType<MySvgView>("mywindow", 1, 0, "SvgView");
    qmlRegisterType<MyMessageBox>("mywindow", 1, 0, "MessageBox");
   
    QSettings mysettings(QDir::homePath ()+"/webqq/.config.ini", QSettings::IniFormat);
    Utility *utility=Utility::createUtilityClass ();
    utility->initUtility (&mysettings, &engine);
    
    QQmlComponent component0(&engine, "./qml/QQApi.qml");
    QQCommand *qqapi = qobject_cast<QQCommand *>(component0.create ());
    engine.rootContext ()->setContextProperty ("myqq", qqapi);
    
    QQmlComponent component(&engine, "./qml/SystemTray.qml");
    SystemTrayIcon *systemTray = qobject_cast<SystemTrayIcon *>(component.create ());
    systemTray->setParent (Utility::createUtilityClass ());//不设置父对象会导致程序退出后托盘还存在的问题
    engine.rootContext ()->setContextProperty ("systemTray", systemTray);//将程序托盘注册过去
    
    engine.load(QUrl(QStringLiteral("qml/LoginPage/main.qml")));
 
    return app.exec();
}
