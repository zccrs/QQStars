#include "qqstars.h"
#include "utility.h"
#include <QJsonDocument>
#include <QSettings>
#include "aes.h"

QQCommand::QQCommand(QQuickItem *parent) :
    QQuickItem(parent)
{
    setUserQQ (Utility::createUtilityClass ()->getValue ("mainqq","").toString ());
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
                if( obj["retcode"].isDouble () ){
                    int retcode = obj["retcode"].toInt ();
                    if( retcode==0 ){
                        QJsonArray arr = obj["result"].toArray ();
                        foreach (QJsonValue temp, arr) {
                            obj = temp.toObject ();
                            QString poll_type=obj["poll_type"].toString ();
                            obj = obj["value"].toObject ();
                            if( poll_type=="message" ){
                                analysisMessage (obj);
                            }else if( poll_type=="input_notify" ){
                                analysisInputNotify (obj);
                            }else if( poll_type=="buddies_status_change" ){
                                analysisFriendStatusChanged(obj);
                            }
                        }
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

QMap<QString, QString> QQCommand::analysisBasicData(QJsonObject obj)
{
    QString from_uin = QString::number ((quint64)obj["from_uin"].toDouble ());
    QString msg_id = QString::number (obj["msg_id"].toInt ());
    QString msg_id2 = QString::number (obj["msg_id2"].toInt ());
    QString msg_type = QString::number (obj["msg_type"].toInt ());
    QString reply_ip = QString::number ((quint64)obj["reply_ip"].toDouble ());
    QString to_uin = QString::number ((quint64)obj["to_uin"].toDouble ());

    qDebug()<<"\n消息发送者的uin是："+from_uin;
    qDebug()<<"消息id1是："+msg_id;
    qDebug()<<"消息id2是："+msg_id2;
    qDebug()<<"消息类型是："+msg_type;
    qDebug()<<"回复的ip地址是："+reply_ip;
    qDebug()<<"接收方的uin是："+to_uin;
    qDebug()<<"来自"+getValue (from_uin+"nick", from_uin).toString ()+"的消息";
    
    QMap<QString, QString> map;
    map.insert ("from_uin", from_uin);
    map.insert ("msg_id", msg_id);
    map.insert ("msg_id2", msg_id2);
    map.insert ("msg_type", msg_type);
    map.insert ("reply_ip", reply_ip);
    map.insert ("to_uin", to_uin);
    return map;
}

void QQCommand::analysisMessage(QJsonObject obj)
{
    analysisBasicData (obj);
    qDebug()<<"是聊天消息";
    QJsonArray content = obj["content"].toArray ();
    foreach (QJsonValue temp2, content) {
        if(temp2.isArray ()){
            QJsonArray font = temp2.toArray ();
            foreach (QJsonValue temp3, font) {
                if(temp3.isObject ()){
                    obj = temp3.toObject ();
                    QString size = QString::number (obj["size"].toInt ());
                    QString color = obj["color"].toString ();
                    QJsonArray style = obj["style"].toArray ();
                    bool bold = (bool)style[0].toInt ();//加黑
                    bool italic = (bool)style[1].toInt ();//斜体
                    bool underline = (bool)style[2].toInt ();//下划线
                    
                    QString name = obj["name"].toString ();
                    
                    qDebug()<<"所使用字体是："+name;
                    qDebug()<<"字体大小是："+size;
                    qDebug()<<"字体颜色是："+color;
                    qDebug()<<"黑体："<<bold;
                    qDebug()<<"斜体："<<italic;
                    qDebug()<<"下划线："<<underline;
                }
            }
        }else if(temp2.isString ()){
            qDebug()<<"消息内容是："+temp2.toString ();
        }
    }
}

void QQCommand::analysisInputNotify(QJsonObject obj)
{
    QMap<QString, QString> map = analysisBasicData (obj);
    qDebug()<<"是输入状态的消息";
    qDebug()<<getValue (map["from_uin"]+"nick", map["from_uin"]).toString ()+"正在输入";
}

void QQCommand::analysisFriendStatusChanged(QJsonObject obj)
{
    
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

void QQCommand::setValue(const QString &key, const QVariant &value, const QString & userQQ)
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    mysettings.setValue (key, value);
}

QVariant QQCommand::getValue(const QString &key, const QVariant &defaultValue, const QString & userQQ ) const
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    return mysettings.value (key, defaultValue);
}

void QQCommand::removeValue(const QString &key, const QString & userQQ )
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    mysettings.remove (key);
}

QString QQCommand::aesEncrypt(const QString &content, const QString &key)
{
    if(content==""||key=="")
        return content;
        
    AES aes((unsigned char*)key.toLatin1 ().data ());
    char miwen_hex[1024];
    aes.Cipher(content.toLatin1 ().data (), miwen_hex);
    return QString(miwen_hex);
}

QString QQCommand::aesUncrypt(const QString &content_hex, const QString &key)
{
    if(content_hex==""||key=="")
        return content_hex;
    
    AES aes((unsigned char*)key.toLatin1 ().data ());
    char result[1024];
    aes.InvCipher(content_hex.toLatin1 ().data (), result);
    return QString(result);
}
