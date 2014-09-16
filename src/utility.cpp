#include "utility.h"
#include <QDebug>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QLabel>
#include <QThread>
#include "mynetworkaccessmanagerfactory.h"
#include "threaddownloadimage.h"

Utility *Utility::createUtilityClass()
{
    static Utility my_utility;
    return &my_utility;
}

Utility::Utility(QObject *parent) :
    QObject(parent)
{
    qDebug()<<"调用了utility的构造函数";
    qmlRegisterType<UtilityPrivate>("utility", 1, 0, "Utility");
    
    mysettings = new QSettings("settings.ini", QSettings::IniFormat, this);
    int temp1 = mysettings->value ("proxyType", QNetworkProxy::NoProxy).toInt ();
    QString temp2 = mysettings->value ("proxyLocation", "").toString ();
    QString temp3 = mysettings->value ("proxyPort", "").toString ();
    QString temp4 = mysettings->value ("proxyUsername", "").toString ();
    QString temp5 = mysettings->value ("proxyPassword", "").toString ();
    
    setApplicationProxy (temp1, temp2, temp3, temp4, temp5);
    
    socket = new MyWebSocket(this);
    
    download_image = new ThreadDownloadImage(this);
    
    old_pos = QPoint(-1,-1);
}

Utility::~Utility()
{
    qDebug()<<"我擦，Utility竟然被销毁了！";
}

void Utility::emitDesktopPosChanged()
{
    QPoint temp = QCursor::pos ();
    if( old_pos!= temp){
        old_pos = temp;
        emit mouseDesktopPosChanged (temp);
    }
}

void Utility::consoleLog(QString str)
{
    qDebug()<<"c++:"+str;
}

QString Utility::getCookie(QString cookieName)
{
    QList<QNetworkCookie> temp = NetworkCookieJar::GetInstance ()->cookies ();
    foreach( QNetworkCookie cookie, temp ) {
        if( cookie.name () == cookieName)
            return cookie.value ();
    }
    return "";
}

QQmlApplicationEngine *Utility::qmlEngine()
{
    return engine;
}

void Utility::setQmlEngine(QQmlApplicationEngine *new_engine)
{
    engine = new_engine;
    engine->rootContext ()->setContextProperty ("utility", this);
    engine->rootContext()->setContextProperty("screen", QApplication::screens ()[0]);
}

QPoint Utility::mouseDesktopPos()
{
    return QCursor::pos ();
}

void Utility::setValue(const QString &key, const QVariant &value)
{
    mysettings->setValue (key, value);
}

QVariant Utility::getValue(const QString &key, const QVariant &defaultValue) const
{
    return mysettings->value (key, defaultValue);
}

void Utility::loadQml(QUrl url)
{
    if(engine)
        engine->load (url);
}

void Utility::downloadImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName)
{
    download_image->getImage (callbackFun, url, savePath, saveName);
}

void Utility::socketSend(QJSValue callbackFun, QUrl url, QByteArray data)
{
    socket->send (callbackFun, url, data);
}

void Utility::setApplicationProxy(int type, QString location, QString port, QString username, QString password)
{
    QNetworkProxy proxy;
    proxy.setType((QNetworkProxy::ProxyType)type);
    proxy.setHostName(location);
    proxy.setPort(port.toUShort ());
    proxy.setUser (username);
    proxy.setPassword (password);
    QNetworkProxy::setApplicationProxy(proxy);
}
