#include <QApplication>
#include <QDebug>
#include <QNetworkProxy>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QScreen>
#include "systemtrayicon.h"
#include "utility.h"
#include "mywindow.h"
#include "mywebsocket.h"
#include "qq.h"
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
    
    //QNetworkProxy proxy;
    //proxy.setType(QNetworkProxy::HttpProxy);
    //proxy.setHostName("localhost");
    //proxy.setPort(8888);
    //QNetworkProxy::setApplicationProxy(proxy);
    
    qmlRegisterType<MyWindow>("mywindow", 1,0, "MyQuickWindow");
    qmlRegisterType<SystemTrayIcon>("mywindow", 1,0, "MySystemTrayIcon");
    qmlRegisterType<MyMenu>("mywindow", 1,0, "MyMenu");
    qmlRegisterType<MenuSeparator>("mywindow", 1,0, "MenuSeparator");
    qmlRegisterType<MyMenuItem>("mywindow", 1,0, "MyMenuItem");
    qmlRegisterType<QQCommand>("utility", 1,0, "QQ");
    qmlRegisterType<MyImage>("mywindow", 1,0, "MyImage");
    qmlRegisterType<MySvgView>("mywindow", 1, 0, "SvgView");
   
    QQmlApplicationEngine *engine = Utility::createUtilityClass ()->qmlEngine();
    QQmlComponent component0(engine, "./qml/QQApi.qml");
    QQCommand *qqapi = qobject_cast<QQCommand *>(component0.create ());
    engine->rootContext ()->setContextProperty ("myqq", qqapi);
    
    QQmlComponent component(engine, "./qml/SystemTray.qml");
    SystemTrayIcon *systemTray = qobject_cast<SystemTrayIcon *>(component.create ());
    systemTray->setParent (Utility::createUtilityClass ());//不设置会导致程序退出后托盘还存在的问题
    engine->rootContext ()->setContextProperty ("systemTray", systemTray);//将程序托盘注册过去
    
    engine->load(QUrl(QStringLiteral("qml/LoginPage/main.qml")));
    return app.exec();
}
