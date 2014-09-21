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
    
public:
    explicit MySocket(QObject *parent = 0);
    enum RequestStatus{
        Idle,//初始状态
        Busy//请求中
    };
    
    NetworkAccessManager *getNetworkAccessManager();
private:
    NetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *m_reply;
    
    RequestStatus m_status;
    RequestStatus status();
    void setStatus( RequestStatus new_status );
    
    QQueue<QJSValue> queue_callbackFun;
    QQueue<QUrl> queue_url;
    QQueue<QByteArray> queue_data;

private slots:
    void finished( QNetworkReply *reply );
    void send();
signals:
    void statusChanged();
public slots:
    void send( QJSValue callbackFun, QUrl url, QByteArray data="" );
    QString errorString();
    void setRawHeader(const QByteArray &headerName, const QByteArray &value);
};

#endif // MYSOCKET_H
