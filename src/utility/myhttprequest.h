#ifndef MYHTTPREQUEST_H
#define MYHTTPREQUEST_H

#include <QObject>
#include <QtNetwork>
#include <QQueue>
#if(QT_VERSION>=0x050000)
#include <QJSValue>
#include <QJSEngine>
#else
#include <QScriptValue>
#include <QScriptEngine>
#endif

class NetworkAccessManager;
class MyHttpRequest : public QObject
{
    Q_OBJECT
    Q_PROPERTY( RequestStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(RequestStatus)

public:
    explicit MyHttpRequest(QObject *parent = 0);
    enum RequestStatus{
        Idle,//初始状态
        Busy//请求中
    };

    const NetworkAccessManager *getNetworkAccessManager();
    QNetworkRequest *getNetworkRequest();
protected:
    NetworkAccessManager *manager;
    QNetworkRequest request;
    QPointer<QNetworkReply> m_reply;
    
    RequestStatus m_status;
    RequestStatus status();
    void setStatus( RequestStatus new_status );
    
    enum ReplyType{//回调的几种方式
        CallbackFun,
        ConnectSlot
    };
    struct Data{
        ReplyType replyType;
#if(QT_VERSION>=0x050000)
        QJSValue callbackFun;
#else
        QScriptValue callbackFun;
#endif
        QObject* caller;
        QByteArray slotName;
    };
    struct requestData{
        QUrl url;
        QByteArray data;
    };

    QQueue<Data> queue_replyData;
    QQueue<requestData> queue_requestData;
    
    void send(QObject *caller, QByteArray slotName, QUrl url, QByteArray data="", bool highRequest=false );//highRequest记录是否为高级的网络请求
#if(QT_VERSION>=0x050000)
    void send( QJSValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
#else
    void send( QScriptValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
#endif
protected slots:
    virtual void finished( QNetworkReply *reply );
    void send();
signals:
    void statusChanged();
public slots:
    void get(QObject *caller, QByteArray slotName, QUrl url, bool highRequest=false );
    void post(QObject *caller, QByteArray slotName, QUrl url, QByteArray data="", bool highRequest=false );
#if(QT_VERSION>=0x050000)
    void get(QJSValue callbackFun, QUrl url, bool highRequest=false );
    void post(QJSValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
#else
    void get(QScriptValue callbackFun, QUrl url, bool highRequest=false );
    void post(QScriptValue callbackFun, QUrl url, QByteArray data="", bool highRequest=false );
#endif
    void abort();//取消当前网络请求
    QString errorString();
};

class MyHttpRequestPrivate : public MyHttpRequest
{
    Q_OBJECT
private:
#if(QT_VERSION>=0x050000)
    explicit MyHttpRequestPrivate(QNetworkRequest request, QJSValue callbackFun, QUrl url, QByteArray data);
#else
    explicit MyHttpRequestPrivate(QNetworkRequest request, QScriptValue callbackFun, QUrl url, QByteArray data);
#endif
    explicit MyHttpRequestPrivate(QNetworkRequest request, QObject *caller, QByteArray slotName, QUrl url, QByteArray data);
    friend class MyHttpRequest;
private slots:
    void finished( QNetworkReply *reply );
};

#endif // MYHTTPREQUEST_H
