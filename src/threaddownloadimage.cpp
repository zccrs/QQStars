#include "threaddownloadimage.h"
#include <QDebug>
#include <QTimer>
#include "utility.h"

ThreadDownloadImage::ThreadDownloadImage(QObject *parent) :
    QObject(parent)
{
    m_status = Idle;
    connect (&imageManager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinish(QNetworkReply*)));
}

ThreadDownloadImage::~ThreadDownloadImage()
{
    qDebug()<<"我擦，下载图片被销毁了";
}

void ThreadDownloadImage::downloadFinish(QNetworkReply *replys)
{
    if(replys->error() == QNetworkReply::NoError)
    {
        QString saveName = queue_saveName.dequeue ();
        QString savePath = queue_savePath.dequeue ();
        QJSValue callbackFun = queue_callbackFun.dequeue ();
        
        if( replys->rawHeader ("Connection")=="close" ) {
             getImage (callbackFun, QUrl(replys->rawHeader ("Location")), saveName, savePath);//继续请求
        }else{
            QString imageName = savePath;
            QDir dir(imageName);
            if( !dir.exists () )
                dir.mkpath (imageName);
            imageName+="/"+saveName+".png";
            QByteArray temp=replys->readAll();
            QImage image;
            image.loadFromData(temp);
            if( !image.save (imageName) )
                qDebug()<<"图片保存失败";
            QJSValueList list;
            list<<QJSValue(savePath)<<QJSValue(saveName);
            callbackFun.call (list);
        }
    }else{
        QJSValueList list;
        list.append (QJSValue("error"));
        queue_callbackFun.dequeue ().call (list);
    }
    getImage ();//继续请求
}

void ThreadDownloadImage::getImage()
{
    if( queue_url.count ()>0 ){
        setStatus (Busy);
        request.setUrl (queue_url.dequeue ());
        imageManager.get (request);
    }else
        setStatus (Idle);
}

void ThreadDownloadImage::getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName)
{
    queue_callbackFun<<callbackFun;
    queue_url<<url;
    queue_saveName<<saveName;
    queue_savePath<<savePath;
    
    if( status ()==Idle ){
        getImage();
    }
}

void ThreadDownloadImage::setStatus(ThreadDownloadImage::RequestStatus arg)
{
    if (m_status != arg) {
        m_status = arg;
        emit statusChanged(arg);
    }
}
