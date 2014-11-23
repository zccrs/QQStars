#include "downloadimage.h"
#include <QDebug>
#include <QTimer>
#include "utility.h"
#include "myhttprequest.h"

DownloadImage::DownloadImage(QObject *parent):
    QObject(parent)
{
    httpRequest = new MyHttpRequest(this);
}

DownloadImage::DownloadImage(MyHttpRequest *http, QObject *parent):
    QObject(parent)
{
    httpRequest = http;
}

MyHttpRequest *DownloadImage::getHttpRequest()
{
    return httpRequest;
}

QString DownloadImage::imageFormatToString(const QByteArray &array)
{
    QByteArray str = array.toHex ();
    
    if(str.mid (2,6)=="504e47")
        return "png";
    if(str.mid (12,8)=="4a464946")
        return "jpg";
    if(str.left (6)=="474946")
        return "gif";
    if(str.left (4)=="424d")
        return "bmp";
    return "";
}

void DownloadImage::downloadFinished(QNetworkReply *replys)
{
    Data data = queue_data.dequeue ();
    ReplyType type=data.replyType;
    ErrorType error=NoError;
    QString saveName = data.saveName;
    QString savePath = data.savePath;
    QString format = ".";
    
    if(replys->error() == QNetworkReply::NoError)
    {
        QString imageName = savePath;
        QDir dir(imageName);
        if( !dir.exists () )
            dir.mkpath (imageName);
        QByteArray temp=replys->readAll();
        imageName+="/"+saveName;
        format += imageFormatToString (temp);
        if(format!="."){
            imageName.append (format);
            qDebug()<<"DownloadImage：要保存的图片名为："<<imageName;
        }else{
            qDebug()<<"DownloadImage:未知的图片格式"<<temp.toHex ();
            error = NotSupportFormat;
            return;
        }
        QFile file(imageName);
        if(file.open (QIODevice::WriteOnly)){
            file.write (temp);//储存图片
            file.close ();
        }else{
            qDebug()<<"DownloadImage:图片保存失败"<<file.errorString ();
            error = SaveError;
        }
    }else{
        error = DownloadError;
    }
    
    if(type == CallbackFun){
#if(QT_VERSION>=0x050000)
        QJSValueList list;
        list<<QJSValue((int)error)<<QJSValue(savePath)<<QJSValue(saveName+format);
        data.callbackFun.call (list);
#else
        QScriptValueList list;
        list<<QScriptValue((int)error)<<QScriptValue(savePath)<<QScriptValue(saveName+format);
        data.callbackFun.call (QScriptValue(), list);
#endif

    }else if(type == ConnectSlot){
        bool ok=QMetaObject::invokeMethod (data.caller, data.slotName,
                                           Q_ARG(DownloadImage::ErrorType, error),
                                           Q_ARG(QString, savePath), 
                                           Q_ARG(QString, saveName+format));
        if(!ok)
            qDebug()<<"DownloadImage:调用槽"<<data.slotName<<"出错";
    }
}

#if(QT_VERSION>=0x050000)
void DownloadImage::getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName)
#else
void DownloadImage::getImage(QScriptValue callbackFun, QUrl url, QString savePath, QString saveName)
#endif
{
    bool isFun=false;
#if(QT_VERSION>=0x050000)
    isFun=callbackFun.isCallable ();
#else
    isFun = callbackFun.isFunction ();
#endif
    if((!isFun)||url.toString ()=="")
        return;
    
    Data data;
    data.replyType = CallbackFun;
    data.callbackFun = callbackFun;
    data.savePath = savePath;
    data.saveName = saveName;
    queue_data<<data;
    
    httpRequest->get (this, SLOT(downloadFinished(QNetworkReply*)), url);//去获取
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
        httpRequest->get (this, SLOT(downloadFinished(QNetworkReply*)), url);//去获取
    }else{
        qDebug()<<"DownloadImage:"<<slotName<<"不是槽函数";
    }
}
