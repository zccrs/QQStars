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
    
    mysettings = NULL;
    engine = NULL;
    
    socket = new MyWebSocket(this);
    
    download_image = new ThreadDownloadImage(this);
    
    old_pos = QPoint(-1,-1);
}

Utility::~Utility()
{
    qDebug()<<"我擦，Utility竟然被销毁了！";
}

char Utility::numToStr(int num)
{
    QByteArray str="QWERTYUIOP[]\\ASDFGHJKL;'ZXCVBNM,./qwertyuiop{}|asdfghjkl:\"zxcvbnm<>?";
    return str[num%str.size ()];
}

QByteArray Utility::strZoarium(const QString &str)
{
    QByteArray result="";
    for(int i=0;i<str.size ();++i){
        QChar ch = str[i];
        int ch_ascii = (int)ch.toLatin1 ();
        if(ch.isLetter ()){//如果是字母
            result.append (numToStr (ch_ascii)).append (QByteArray::number (ch_ascii)).append (numToStr (ch_ascii*2));
        }else{//如果是数字
            result.append (ch);
        }
    }
    return result;
}

QByteArray Utility::unStrZoarium(const QString &str)
{
    QByteArray result="";
    for(int i=0;i<str.size ();){
        QChar ch = str[i];
        if(ch.isNumber ()){//如果是数字
            result.append (ch);
            i++;
        }else{//如果是其他
            QRegExp regexp("[^0-9]");
            int pos = str.indexOf (regexp, i+1);
            if(pos!=0){
                char num = (char)str.mid (i+1, pos-i-1).toInt ();
                result.append (num);
                i=pos+1;
            }else{
                i++;
            }
        }
    }
    return result;
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
    if(new_engine){
        engine = new_engine;
        engine->rootContext ()->setContextProperty ("utility", this);
        engine->rootContext()->setContextProperty("screen", QApplication::screens ()[0]);
    }
}

void Utility::initUtility(QSettings *settings, QQmlApplicationEngine *qmlEngine)
{
    setQSettings (settings);
    setQmlEngine (qmlEngine);
}

QPoint Utility::mouseDesktopPos()
{
    return QCursor::pos ();
}

void Utility::setQSettings(QSettings *settings)
{
    if(settings)
        mysettings = settings;
}

void Utility::setValue(const QString &key, const QVariant &value)
{
    if( mysettings )
        mysettings->setValue (key, value);
    else
        qDebug()<<"mysetting=NULL";
}

QVariant Utility::getValue(const QString &key, const QVariant &defaultValue) const
{
    if( mysettings )
        return mysettings->value (key, defaultValue);
    else{
        qDebug()<<"mysetting=NULL";
        return QVariant("");
    }
}

void Utility::removeValue(const QString &key)
{
    if( mysettings )
        mysettings->remove (key);
    else
        qDebug()<<"mysetting=NULL";
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

QString Utility::stringEncrypt(const QString &content, const QString &key)
{
    if(content==""||key=="")
        return content;
    
    QByteArray data = strZoarium (content.toUtf8 ().toHex ());
    int data_size = data.size ();
    QByteArray mykey = strZoarium (key.toUtf8 ().toHex ());
    int key_size = mykey.size ();
    QByteArray temp="";
    for(int i=0;i<data_size;++i){
        temp.append ((char)(int)data[i]+(int)mykey[i%key_size]);
    }
    return temp;
}

QString Utility::stringUncrypt(const QString &content_hex, const QString &key)
{
    if(content_hex==""||key=="")
        return content_hex;
    
    QByteArray data = content_hex.toLatin1 ();
    int data_size = data.size ();
    QByteArray mykey = strZoarium (key.toUtf8 ().toHex ());
    int key_size = mykey.size ();
    QByteArray str;
    for(int i=0;i<data_size;++i){
        str.append ((char)(int)data[i]-(int)mykey[i%key_size]);
    }
    return QByteArray::fromHex (unStrZoarium (str));
}
