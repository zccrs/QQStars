#include "qqstars.h"
#include "utility.h"
#include <QJsonDocument>
#include <QSettings>

QQCommand::QQCommand(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_loginStatus = Offline;
    m_userStatusToString = "online";
    connect (this, SIGNAL(userStatusChanged()), SIGNAL(userStatusToStringChanged()));
    
    request.setUrl (QUrl("http://d.web2.qq.com/channel/poll2"));
    request.setRawHeader ("Origin", "http://d.web2.qq.com");
    request.setRawHeader ("Accept", "*/*");
    request.setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    request.setRawHeader ("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader ("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36 LBBROWSER");
    poll2_timer.setInterval (20000);
    connect (&poll2_timer, SIGNAL(timeout()), SLOT(beginPoll2()));
    connect (&manager, SIGNAL(finished(QNetworkReply*)), SLOT(poll2Finished(QNetworkReply*)));

}

void QQCommand::beginPoll2()
{
    manager.post (request, poll2_data);
}

void QQCommand::poll2Finished(QNetworkReply *replys)
{
    if(replys->error ()==QNetworkReply::NoError) {
        QByteArray array = replys->readAll ();
        emit poll2ReData (array);
        QJsonParseError json_error;
        QJsonDocument document = QJsonDocument::fromJson (array, &json_error);
        if(json_error.error == QJsonParseError::NoError) 
        {
            if( document.isObject () ){
                QJsonObject obj = document.object ();
                if( obj.value ("retcode").isDouble () ){
                    int retcode = obj.value ("retcode").toInt ();
                    if( retcode==0 ){
                        beginPoll2();
                    }else if(retcode==102){
                        beginPoll2();
                    }
                }
            }else if(document.isArray ()){
                QJsonArray arr = document.array ();
                qDebug()<<arr.count ();
            }
        }
    }
}

QQCommand::QQStatus QQCommand::userStatus()
{
    return qq_status;
}

void QQCommand::setUserStatus(QQCommand::QQStatus new_status)
{
    if( new_status!=qq_status ) {
        qq_status = new_status;
        switch(qq_status)
        {
        case Online:
            m_userStatusToString = "online";
            break;
        case Callme:
            m_userStatusToString = "callme";
            break;
        case Away:
            m_userStatusToString = "away";
            break;
        case Busy:
            m_userStatusToString = "busy";
            break;
        case Silent:
            m_userStatusToString = "silent";
            break;
        case Hidden:
            m_userStatusToString = "hidden";
            break;
        case Offlineing:
            m_userStatusToString = "offline";
            break;
        default:break;
        }
        emit userStatusChanged ();
    }
}

void QQCommand::setLoginStatus(QQCommand::LoginStatus arg)
{
    if (m_loginStatus != arg) {
        m_loginStatus = arg;
        emit loginStatusChanged(arg);
    }
}

void QQCommand::startPoll2(QByteArray data)
{
    poll2_data = data;
    //poll2_timer.start ();
    beginPoll2();
}

void QQCommand::showWarningInfo(QString message)
{
    emit error (message);
    QQmlComponent component(Utility::createUtilityClass ()->qmlEngine (), "./qml/WarningWindow.qml");
    QObject *obj = component.create ();
    obj->setProperty ("text", QVariant(message));
}

void QQCommand::downloadImage(QUrl url, QString uin, QString imageSize, QJSValue callbackFun)
{
    QString path = QDir::homePath ()+"/webqq/"+userQQ ()+"/"+uin;
    Utility::createUtilityClass ()->downloadImage (callbackFun, url, path, "avatar-"+imageSize);
}
