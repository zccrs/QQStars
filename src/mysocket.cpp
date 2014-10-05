#include "mysocket.h"

MySocket::MySocket(QObject *parent) :
    QObject(parent)
{
    m_status = Idle;
    manager = new NetworkAccessManager(this);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    connect (manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*)));
}

MySocket::RequestStatus MySocket::status()
{
    return m_status;
}

void MySocket::setStatus(MySocket::RequestStatus new_status)
{
    if( new_status!=m_status ) {
        m_status = new_status;
        emit statusChanged();
    }
}

NetworkAccessManager *MySocket::getNetworkAccessManager()
{
    return manager;
}

void MySocket::finished(QNetworkReply *reply)
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

void MySocket::send(QJSValue callbackFun, QUrl url, Priority priority, QByteArray data)
{
    if(priority==High){
        qDebug()<<"高优先级的网络请求";
        new MySocketPrivate(callbackFun, url, data);//进行高优先级的网络请求
    }else if(priority==Low){
        queue_callbackFun<<callbackFun;
        queue_url<<url;
        queue_data<<data;
        if(status ()==Idle){
            QTimer::singleShot (10, this, SLOT(send()));//不然可能会堵塞ui线程
        }
    }
}

void MySocket::abort()
{
    if(!m_reply.isNull ()){
        m_reply->abort ();
    }
}

void MySocket::send()
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

QString MySocket::errorString()
{
    return m_reply->errorString ();
}

void MySocket::setRawHeader(const QByteArray &headerName, const QByteArray &value)
{
    request.setRawHeader (headerName, value);
}


MySocketPrivate::MySocketPrivate(QJSValue callbackFun, QUrl url, QByteArray data):
    MySocket(0)
{
    qDebug()<<"创建了高优先级的网络请求";
    queue_callbackFun<<callbackFun;
    queue_url<<url;
    queue_data<<data;
    QTimer::singleShot (10, this, SLOT(send()));//不然可能会堵塞ui线程
}

void MySocketPrivate::finished(QNetworkReply *reply)
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
    deleteLater ();//销毁自己
}
