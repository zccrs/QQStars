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
    ReplyType type=queue_replyType.dequeue ();
    if( reply->error () == QNetworkReply::NoError) {
        if(type==CallbackFun){
            QJSValueList list;
            list.append (QJSValue(false));
            list.append (QJSValue(QString(reply->readAll ())));
            queue_callbackFun.dequeue ().call (list);
        }else if(type==ConnectSlot){
            QObject* obj = queue_caller.dequeue ();
            QByteArray slotName = queue_slotName.dequeue ();
            if(!QMetaObject::invokeMethod (obj, slotName.data ()))
                qDebug()<<"调用槽"+slotName+"失败";
        }
    }else{
        if(type==CallbackFun){
            QJSValueList list;
            list.append (QJSValue(true));
            list.append (QJSValue(reply->errorString ()));
            queue_callbackFun.dequeue ().call (list);
        }else if(type==ConnectSlot){
            
        }
    }
    send();//继续请求
}

void MySocket::send(QJSValue callbackFun, QUrl url, QByteArray data, MySocket::Priority priority)
{
    if((!callbackFun.isCallable ())||url.toString ()=="")
        return;
    if(priority==High){
        qDebug()<<"高优先级的网络请求";
        new MySocketPrivate(callbackFun, url, data);//进行高优先级的网络请求
    }else if(priority==Low){
        queue_replyType<<CallbackFun;
        queue_callbackFun<<callbackFun;
        queue_url<<url;
        queue_data<<data;
        if(status ()==Idle){
            QTimer::singleShot (10, this, SLOT(send()));//不然可能会堵塞ui线程
        }
    }
}

void MySocket::get(QObject *caller, QByteArray slotName, QUrl url, MySocket::Priority priority)
{
    send(caller, slotName, url, "", priority);
}

void MySocket::post(QObject *caller, QByteArray slotName, QUrl url, QByteArray data, MySocket::Priority priority)
{
    send(caller, slotName, url, data, priority);
}

void MySocket::send(QObject *caller, QByteArray slotName, QUrl url, QByteArray data, MySocket::Priority priority)
{
    if(caller==NULL||slotName==""||url.toString ()=="")
        return;
    if(priority==High){
        qDebug()<<"高优先级的网络请求";
        new MySocketPrivate(caller, slotName, url, data);//进行高优先级的网络请求
    }else if(priority==Low){
        queue_replyType<<ConnectSlot;
        queue_caller<<caller;
        queue_slotName<<slotName;
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
    send(callbackFun, url, data, Low);
}

MySocketPrivate::MySocketPrivate(QObject *caller, QByteArray slotName, QUrl url, QByteArray data):
    MySocket(0)
{
    qDebug()<<"创建了高优先级的网络请求";
    send(caller, slotName, url, data, Low);
}

void MySocketPrivate::finished(QNetworkReply *reply)
{
    MySocket::finished (reply);
    deleteLater ();//销毁自己
}
