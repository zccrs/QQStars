#include "utility.h"
#include <QDebug>
#include <QtConcurrent>
#include <QApplication>
#include "mynetworkaccessmanagerfactory.h"
#include "downloadimage.h"
#include "myhttprequest.h"

Utility *Utility::createUtilityClass()
{
    static Utility my_utility;
    return &my_utility;
}

Utility::Utility(QObject *parent) :
    QObject(parent)
{
    qmlRegisterType<UtilityPrivate>("utility", 1, 0, "Utility");

    http_request = new MyHttpRequest(this);
    download_image = new DownloadImage(this);
    connect (&networkConfigurationManager, &QNetworkConfigurationManager::onlineStateChanged,
             this, &Utility::networkOnlineStateChanged);
}

Utility::~Utility()
{
}

char Utility::numToStr(int num)
{
    QByteArray str="QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm";
    return str[num%str.size ()];
}

QByteArray Utility::strZoarium(const QByteArray &str)
{
    QByteArray result;
    for(int i=0;i<str.size ();++i){
        char ch = (char)str[i];
        int ch_ascii = (int)ch;
        if(ch<='9'&&ch>='0'){//如果是数字
            result.append (ch);
        }else{//如果不是数字
            if(ch_ascii>=0)
                result.append (numToStr (ch_ascii)).append (QByteArray::number (ch_ascii)).append (numToStr (ch_ascii*2));
        }
    }
    return result;
}

QByteArray Utility::unStrZoarium(const QByteArray &str)
{
    QByteArray result="";
    for(int i=0;i<str.size ();){
        char ch = (char)str[i];
        if(ch<='9'&&ch>='0'){//如果是数字
            result.append (ch);
            i++;
        }else{//如果是其他
            QRegExp regexp("[^0-9]");
            int pos = QString(str).indexOf (regexp, i+1);
            if(pos>0){
                int num = str.mid (i+1, pos-i-1).toInt ();
                if(num>=0)
                    result.append ((char)num);
                i=pos+1;
            }else{
                //qDebug()<<"数据有错";
                i++;
            }
        }
    }
    return result;
}

QByteArray Utility::fillContent(const QByteArray &str, int length)
{
    if(length>0){
        QByteArray fill_size = QByteArray::number (length);
        if(fill_size.size ()==1)
            fill_size="00"+fill_size;
        else if(fill_size.size ()==2)
            fill_size="0"+fill_size;
        for(int i=0;i<length;++i){
            fill_size.append ("0");
        }
        return fill_size+str;
    }else{
        return "000"+str;
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

MyHttpRequest *Utility::getHttpRequest()
{
    return http_request;
}

DownloadImage *Utility::getDownloadImage()
{
    return download_image;
}

bool Utility::networkIsOnline() const
{
    return networkConfigurationManager.isOnline ();
}

void Utility::setQmlEngine(QQmlApplicationEngine *new_engine)
{
    engine = new_engine;
    if(engine){
        engine->rootContext ()->setContextProperty ("utility", this);
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

QVariant Utility::value(const QString &key, const QVariant &defaultValue) const
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
#if(QT_VERSION>=0x050000)
void Utility::downloadImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName)
{
    download_image->getImage (callbackFun, url, savePath, saveName);
}

void Utility::httpGet(QJSValue callbackFun, QUrl url, bool highRequest)
{
    http_request->get (callbackFun, url, highRequest);
}

void Utility::httpPost(QJSValue callbackFun, QUrl url, QByteArray data, bool highRequest)
{
    http_request->post (callbackFun, url, data, highRequest);
}
#else
void Utility::downloadImage(QScriptValue callbackFun, QUrl url, QString savePath, QString saveName)
{
    download_image->getImage (callbackFun, url, savePath, saveName);
}

void Utility::httpGet(QScriptValue callbackFun, QUrl url, bool highRequest)
{
    http_request->get (callbackFun, url, highRequest);
}

void Utility::httpPost(QScriptValue callbackFun, QUrl url, QByteArray data, bool highRequest)
{
    http_request->post (callbackFun, url, data, highRequest);
}
#endif
void Utility::downloadImage(QObject *caller, QByteArray slotName, QUrl url, QString savePath, QString saveName)
{
    download_image->getImage (caller, slotName, url, savePath, saveName);
}

void Utility::httpGet(QObject *caller, QByteArray slotName, QUrl url, bool highRequest)
{
    http_request->get (caller, slotName, url, highRequest);
}

void Utility::httpPost(QObject *caller, QByteArray slotName, QUrl url, QByteArray data, bool highRequest)
{
    http_request->post (caller, slotName, url, data, highRequest);
}

void Utility::socketAbort()
{
    http_request->abort ();
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

QString Utility::stringEncrypt(const QString &content, QString key)
{
    if(content==""||key=="")
        return content;
    if(key.size ()>256)
        key = key.mid (0,256);//密匙最长256位
    QByteArray data = strZoarium (content.toUtf8 ().toBase64 ());
    int data_size = data.size ();
    QByteArray mykey = strZoarium (key.toLatin1 ().toHex ());
    int key_size = mykey.size ();
    //qDebug()<<data;
    data=fillContent (data, 2*key_size-data_size);//填充字符串
    //qDebug()<<data;
    QByteArray temp="";
    for(int i=0;i<data.size ();++i){
        int ch = (int)data[i]+(int)mykey[i%key_size];
        //qDebug()<<ch<<(int)mykey[i%key_size]<<(int)data[i];
        if(ch>=0)
            temp.append (QString(ch));
    }
    //qDebug()<<temp;
    return QString::fromUtf8 (temp);
}

QString Utility::stringUncrypt(const QString &content, QString key)
{
    if(content==""||key=="")
        return content;
    if(key.size ()>256)
        key = key.mid (0,256);//密匙最长256位
    QByteArray data = content.toLatin1 ();
    QByteArray mykey = strZoarium (key.toLatin1 ().toHex ());
    int key_size = mykey.size ();
    QByteArray temp;

    for(int i=0;i<data.size ();++i){
        int ch = (int)(uchar)data[i]-(int)mykey[i%key_size];
        if(ch>=0){
            temp.append ((char)ch);
        }
    }
    temp = unStrZoarium (temp);
    int fill_size = temp.mid (0, 3).toInt ();
    temp = temp.mid (fill_size+3, temp.size ()-fill_size-3);//除去填充的字符
    
    return QString::fromUtf8 (QByteArray::fromBase64 (temp));
}

bool myRemovePath(QString dirPath, bool deleteHidden, bool deleteSelf)
{
    qDebug()<<"removePath的进程"<<QThread::currentThread ();
    QDir entry (dirPath);
    if(!entry.exists()||!entry.isReadable())
        return false;
    entry.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
    QFileInfoList dirList = entry.entryInfoList();
    bool bHaveHiddenFile = false;

    if(!dirList.isEmpty()) {
        for( int i = 0; i < dirList.size() ; ++i) {
            QFileInfo info = dirList.at(i);
            if(info.isHidden() && !deleteHidden) {
                bHaveHiddenFile = true;
                continue;
            }
            QString path = info.absoluteFilePath();
            if(info.isDir()) {
                if(!myRemovePath(path, deleteHidden, true))
                    return false;
            }else if(info.isFile()) {
                if(!QFile::remove(path))
                    return false;
            }else
                return false;
        }
    }

    if(deleteSelf && !bHaveHiddenFile) {
        if(!entry.rmdir(dirPath)) {
            return false;
        }
    }
    return true;
}

void Utility::removePath(QString dirPath, bool deleteHidden/*=false*/, bool deleteSelf/*=true*/)
{
    qDebug()<<"removePath的调用进程"<<QThread::currentThread ();
    QtConcurrent::run(myRemovePath, dirPath, deleteHidden, deleteSelf);
}
