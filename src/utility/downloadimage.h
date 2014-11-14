#ifndef DOWNLOADIMAGE_H
#define DOWNLOADIMAGE_H

#include <QObject>
#include <QQueue>
#include <QByteArray>
#include <QString>
#include <QImage>
#include <QUrl>
#if(QT_VERSION>=0x050000)
#include <QJSValue>
#include <QJSEngine>
#else
#include <QScriptValue>
#include <QScriptEngine>
#endif

class MyHttpRequest;
class QNetworkReply;
class DownloadImage : public QObject
{
    Q_OBJECT
    Q_ENUMS(ErrorType)
public:
    explicit DownloadImage(QObject *parent = 0);
    explicit DownloadImage(MyHttpRequest* http, QObject *parent = 0);
    MyHttpRequest *getHttpRequest();
    
    static QString imageFormatToString(const QByteArray& array);
    
    enum ErrorType{
        NoError,//无错误
        DownloadError,//下载出错
        SaveError,//保存出错
        NotSupportFormat//不支持的图片格式
    };

private:
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
        QString savePath;
        QString saveName;
    };
    QQueue<Data> queue_data;
    MyHttpRequest* httpRequest;
    
private slots:
    void downloadFinished(QNetworkReply *replys);//当图片下载完成的时候调用
public slots:
#if(QT_VERSION>=0x050000)
    void getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName);
#else
    void getImage(QScriptValue callbackFun, QUrl url, QString savePath, QString saveName);
#endif
    void getImage(QObject *caller, QByteArray slotName, QUrl url, QString savePath, QString saveName);
    /*
     * savePath中最后一个目录后面不要加"/"，saveName不要带后缀
    */
};

#endif // DOWNLOADIMAGE_H
