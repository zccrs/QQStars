#ifndef UTILITY_H
#define UTILITY_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include "mynetworkaccessmanagerfactory.h"
#include "threaddownloadimage.h"
#include "mywebsocket.h"
#include <QTimer>
#include <QPoint>
#include <QSettings>

class UtilityPrivate : public QObject
{
    Q_OBJECT
    Q_ENUMS(ProxyType)
public:
    enum ProxyType {
        DefaultProxy,
        Socks5Proxy,
        NoProxy,
        HttpProxy,
        HttpCachingProxy,
        FtpCachingProxy
    };
};

class Utility : public QObject
{
    Q_OBJECT
public:
    static Utility *createUtilityClass(  );
    
private:
    explicit Utility(QObject *parent = 0);
    ~Utility();
    QQmlApplicationEngine *engine;
    QPoint old_pos;
    QSettings *mysettings;
    
    MyWebSocket *socket;
    ThreadDownloadImage *download_image;

private slots:
    void emitDesktopPosChanged();
public:
    Q_INVOKABLE void consoleLog(QString str);//输出调试信息
    Q_INVOKABLE QString getCookie( QString cookieName );
    QQmlApplicationEngine *qmlEngine();
signals:
    void mouseDesktopPosChanged(QPoint pos);
    void imageDownloadFinish(QString uin, QString path);
public slots:
    void initUtility(QSettings *settings=0, QQmlApplicationEngine *qmlEngine=0);
    void setQmlEngine( QQmlApplicationEngine *new_engine );
    QPoint mouseDesktopPos();
    
    void setQSettings(QSettings *settings);
    void setValue( const QString & key, const QVariant & value);
    QVariant getValue(const QString & key, const QVariant & defaultValue = QVariant()) const;
    void removeValue( const QString & key );
    
    void loadQml( QUrl url );
    void downloadImage( QJSValue callbackFun, QUrl url, QString savePath, QString saveName );
    void socketSend(QJSValue callbackFun, QUrl url, QByteArray data="");
    void setApplicationProxy( int type, QString location, QString port, QString username, QString password );
};

#endif // UTILITY_H
