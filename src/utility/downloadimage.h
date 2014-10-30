#ifndef DOWNLOADIMAGE_H
#define DOWNLOADIMAGE_H

#include <QObject>
#include <QQueue>
#include <QByteArray>
#include <QString>
#include <QImage>
#include <QJSValue>
#include <QUrl>

class MyHttpRequest;
class QNetworkReply;
class DownloadImage : public QObject
{
    Q_OBJECT

public:
    explicit DownloadImage(QObject *parent = 0);
    explicit DownloadImage(MyHttpRequest* http, QObject *parent = 0);
private:
    enum ReplyType{//回调的几种方式
        CallbackFun,
        ConnectSlot
    };
    struct Data{
        ReplyType replyType;
        QJSValue callbackFun;
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
    void getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName);
    void getImage(QObject *caller, QByteArray slotName, QUrl url, QString savePath, QString saveName);
};

#endif // DOWNLOADIMAGE_H
