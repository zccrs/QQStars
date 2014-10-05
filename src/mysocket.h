#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QtNetwork>
#include <QQueue>
#include "mynetworkaccessmanagerfactory.h"

class MySocket : public QObject
{
    Q_OBJECT
    Q_PROPERTY( RequestStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(RequestStatus)
    Q_ENUMS(Priority)
public:
    explicit MySocket(QObject *parent = 0);
    enum RequestStatus{
        Idle,//初始状态
        Busy//请求中
    };
    enum Priority{
        Low,
        High
    };

    NetworkAccessManager *getNetworkAccessManager();
protected:
    NetworkAccessManager *manager;
    QNetworkRequest request;
    QPointer<QNetworkReply> m_reply;
    
    RequestStatus m_status;
    RequestStatus status();
    void setStatus( RequestStatus new_status );
    
    QQueue<QJSValue> queue_callbackFun;
    QQueue<QUrl> queue_url;
    QQueue<QByteArray> queue_data;

protected slots:
    virtual void finished( QNetworkReply *reply );
    void send();
signals:
    void statusChanged();
public slots:
    void send( QJSValue callbackFun, QUrl url, Priority priority, QByteArray data="" );
    void abort();//取消当前网络请求
    QString errorString();
    void setRawHeader(const QByteArray &headerName, const QByteArray &value);
};

class MySocketPrivate : public MySocket
{
    Q_OBJECT
private:
    explicit MySocketPrivate(QJSValue callbackFun, QUrl url, QByteArray data="");
    friend class MySocket;
private slots:
    void finished( QNetworkReply *reply );
};

#endif // MYSOCKET_H
