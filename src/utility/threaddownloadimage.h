#ifndef THREADDOWNLOADIMAGE_H
#define THREADDOWNLOADIMAGE_H

#include <QObject>
#include <QQueue>
#include "mynetworkaccessmanagerfactory.h"
#include <QByteArray>
#include <QString>
#include <QImage>
#include <QQueue>

class ThreadDownloadImage : public QObject
{
    Q_OBJECT
    Q_PROPERTY( RequestStatus status READ status WRITE setStatus NOTIFY statusChanged)
    Q_ENUMS(RequestStatus)
public:
    explicit ThreadDownloadImage(QObject *parent = 0);
    ~ThreadDownloadImage();
    
    enum RequestStatus{
        Idle,//初始状态
        Busy//请求中
    };
    RequestStatus status() const;
    
private:
    NetworkAccessManager imageManager;
    QNetworkRequest request;
    
    QQueue<QJSValue> queue_callbackFun;
    QQueue<QUrl> queue_url;
    QQueue<QString> queue_saveName;
    QQueue<QString> queue_savePath;
    
    RequestStatus m_status;

private slots:
    void downloadFinish(QNetworkReply *replys);//当图片下载完成的时候调用
    void getImage();
signals:
    void imageDownloadFinish(QString path, QString imageSize);
    void statusChanged(RequestStatus arg);
    
public slots:
    void getImage(QJSValue callbackFun, QUrl url, QString savePath, QString saveName);
    void setStatus(RequestStatus arg);
};

#endif // THREADDOWNLOADIMAGE_H
