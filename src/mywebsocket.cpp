#include "mywebsocket.h"

MyWebSocket::MyWebSocket(QObject *parent) :
    QObject(parent)
{
    m_status = Idle;
    manager = new NetworkAccessManager(this);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    connect (manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*)));
}

MyWebSocket::RequestStatus MyWebSocket::status()
{
    return m_status;
}

void MyWebSocket::setStatus(MyWebSocket::RequestStatus new_status)
{
    if( new_status!=m_status ) {
        m_status = new_status;
        emit statusChanged();
    }
}

NetworkAccessManager *MyWebSocket::getNetworkAccessManager()
{
    return manager;
}

void MyWebSocket::finished(QNetworkReply *reply)
{
    if( reply->error () == QNetworkReply::NoError) {
        QJSValueList list;
        list.append (QJSValue(false));
        list.append (QJSValue(QString(reply->readAll ())));
        queue_callbackFun.dequeue ().call (list);
    }else{
        QJSValueList list;
        list.append (QJSValue(true));
        list.append (QJSValue(reply->errorString ()));
        queue_callbackFun.dequeue ().call (list);
    }
    send();//继续请求
}

void MyWebSocket::send(QJSValue callbackFun, QUrl url, QByteArray data)
{
    queue_callbackFun<<callbackFun;
    queue_url<<url;
    queue_data<<data;
    if(status ()==Idle){
        QTimer::singleShot (10, this, SLOT(send()));//不然可能会堵塞ui线程
    }
}

void MyWebSocket::send()
{
    if( queue_url.count ()>0){
        setStatus (Busy);
        request.setUrl (queue_url.dequeue ());
        QByteArray data = queue_data.dequeue ();
        if( data=="" )
            m_reply = manager->get (request);
        else
            m_reply = manager->post (request, data);
    }else
        setStatus (Idle);//设置状态为空闲
}

QString MyWebSocket::errorString()
{
    return m_reply->errorString ();
}

void MyWebSocket::setRawHeader(const QByteArray &headerName, const QByteArray &value)
{
    request.setRawHeader (headerName, value);
}
