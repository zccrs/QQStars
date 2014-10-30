#include "downloadimage.h"
#include <QDebug>
#include <QTimer>
#include "utility.h"
#include "myhttprequest.h"

DownloadImage::DownloadImage(QObject *parent)
{
    DownloadImage(new MyHttpRequest(this), parent);
}

DownloadImage::DownloadImage(MyHttpRequest *http, QObject *parent):
    QObject(parent)
{
    httpRequest = http;
}

void DownloadImage::downloadFinished(QNetworkReply *replys)
{
    Data data = queue_data.dequeue ();
    ReplyType type=data.replyType;
    bool error=false;
    QString saveName = data.saveName;
    QString savePath = data.savePath;
    
    if(replys->error() == QNetworkReply::NoError)
    {
        if( replys->rawHeader ("Connection")=="close" ) {
            if(type == CallbackFun)
                getImage (data.callbackFun, QUrl(replys->rawHeader ("Location")), savePath, saveName);//继续请求
            else if(type == ConnectSlot)
                getImage (data.caller, data.slotName, QUrl(replys->rawHeader ("Location")), savePath, saveName);
            return;
        }else{
            QString imageName = savePath;
            QDir dir(imageName);
            if( !dir.exists () )
                dir.mkpath (imageName);
            imageName+="/"+saveName+".png";
            QByteArray temp=replys->readAll();
            QImage image;
            image.loadFromData(temp);
            if( !image.save (imageName) ){
                qDebug()<<"DownloadImage:图片保存失败";
                error = true;
            }
        }
    }else{
        error = true;
    }
    
    if(error){
        if(type == CallbackFun){
            QJSValueList list;
            list<<QJSValue("error");
            data.callbackFun.call (list);
        }else if(type == ConnectSlot){
            bool ok=QMetaObject::invokeMethod (data.caller, data.slotName, Q_ARG(QString, "error"), Q_ARG(QString, ""));
            if(!ok)
                qDebug()<<"DownloadImage:调用槽"<<data.slotName<<"出错";
        }
    }else{
        if(type == CallbackFun){
            QJSValueList list;
            list<<QJSValue(savePath)<<QJSValue(saveName);
            data.callbackFun.call (list);
        }else if(type == ConnectSlot){
            bool ok=QMetaObject::invokeMethod (data.caller, data.slotName, Q_ARG(QString, savePath), Q_ARG(QString, saveName));
            if(!ok)
                qDebug()<<"DownloadImage:调用槽"<<data.slotName<<"出错";
        }
    }
}

void DownloadImage::getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName)
{
    if((!callbackFun.isCallable ())||url.toString ()=="")
        return;
    
    Data data;
    data.replyType = CallbackFun;
    data.callbackFun = callbackFun;
    data.savePath = savePath;
    data.saveName = saveName;
    queue_data<<data;
    
    httpRequest->get (this, SLOT(downloadFinished(QNetworkReply*)), url, true);//去获取
}

void DownloadImage::getImage(QObject *caller, QByteArray slotName, QUrl url, QString savePath, QString saveName)
{
    if(caller==NULL||slotName==""||url.toString ()=="")
        return;
    
    QRegExp reg("[_A-Za-z][_A-Za-z0-9]*");//提取函数名
    if(reg.indexIn (slotName)>=0){
        slotName = reg.cap (0).toLatin1 ();
        Data data;
        data.replyType = ConnectSlot;
        data.caller = caller;
        data.slotName = slotName;
        data.savePath = savePath;
        data.saveName = saveName;
        queue_data<<data;
        httpRequest->get (this, SLOT(downloadFinished(QNetworkReply*)), url, true);//去获取
    }else{
        qDebug()<<"DownloadImage:"<<slotName<<"不是一个规范的函数，所以不可能是一个槽";
    }
}
