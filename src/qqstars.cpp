#include "qqstars.h"
#include "utility.h"
#include <QJsonDocument>
#include <QSettings>
#include "aes.h"

QQCommand::QQCommand(QQuickItem *parent) :
    QQuickItem(parent)
{
    Utility *utility=Utility::createUtilityClass ();
    int temp1 = utility->getValue ("proxyType", QNetworkProxy::NoProxy).toInt ();
    QString temp2 = utility->getValue ("proxyLocation", "").toString ();
    QString temp3 = utility->getValue ("proxyPort", "").toString ();
    QString temp4 = utility->getValue ("proxyUsername", "").toString ();
    QString temp5 = utility->getValue ("proxyPassword", "").toString ();
    utility->setApplicationProxy (temp1, temp2, temp3, temp4, temp5);
    
    setUserQQ (utility->getValue ("mainqq","").toString ());
    m_loginStatus = Offline;
    m_userStatusToString = "online";
    m_windowScale = 1;
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
    
    loadApi ();//加载api的js文件
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
                                disposeFriendMessage (obj);
                            }else if( poll_type=="input_notify" ){
                                disposeInputNotify (obj);
                            }else if( poll_type=="buddies_status_change" ){
                                disposeFriendStatusChanged(obj);
                            }else if( poll_type=="group_message" ){
                                disposeGroupMessage (obj);//解析群消息
                            }else if( poll_type=="discu_message" ){
                                disposeDiscuMessage (obj);//解析讨论组消息
                            }else if( poll_type=="file_message" ){
                                qDebug()<<"发送文件消息";
                            }else if( poll_type=="av_request" ){
                                qDebug()<<"视频聊天消息";
                            }else if( poll_type=="av_refuse" ){
                                qDebug()<<"取消开视频";
                            }else if( poll_type=="shake_message" ){
                                qDebug()<<"窗口抖动消息";
                            }else{
                                qDebug()<<"其他消息"<<poll_type;
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

void QQCommand::loadApi()
{
    QString fileName = "qml/api.js";
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
        qDebug()<<"打开"+fileName+"失败";
    QString contents = scriptFile.readAll ();
    scriptFile.close();
    jsEngine.evaluate(contents, fileName);
}

void QQCommand::analysisMessage(QJsonObject &obj)
{
    QJsonArray content = obj["content"].toArray ();
    QJsonValue temp2 = content[0];
    if(temp2.isArray ()){
        QJsonArray font = temp2.toArray ();
        foreach (QJsonValue temp3, font) {
            if(temp3.isObject ()){
                obj = temp3.toObject ();
                QString size = doubleToString (obj, "size");
                QString color = obj["color"].toString ();
                QJsonArray style = obj["style"].toArray ();
                bool bold = (bool)style[0].toInt ();//加黑
                bool italic = (bool)style[1].toInt ();//斜体
                bool underline = (bool)style[2].toInt ();//下划线
                
                QString name = obj["name"].toString ();
                
                /*qDebug()<<"所使用字体是："+name;
                qDebug()<<"字体大小是："+size;
                qDebug()<<"字体颜色是："+color;
                qDebug()<<"黑体："<<bold;
                qDebug()<<"斜体："<<italic;
                qDebug()<<"下划线："<<underline;*/
            }
        }
    }
    temp2 = content[1];
    if(temp2.isArray ()){
        QJsonArray array = temp2.toArray ();
        QString array_name = array[0].toString ();
        if(array_name=="cface"){
            foreach (QJsonValue temp3, array) {
                if(temp3.isObject ()){
                    obj = temp3.toObject ();
                    QString file_id = doubleToString (obj, "file_id");
                    QString key = obj["key"].toString ();
                    QString name = obj["name"].toString ();
                    QString server = obj["server"].toString ();
                    qDebug()<<"收到了文件"<<"file_id:"+file_id<<"key:"+key<<"name:"+name<<"server:"+server;
                }
            }
        }else if(array_name=="offpic"){
            foreach (QJsonValue temp3, array) {
                if(temp3.isObject ()){
                    obj = temp3.toObject ();
                    QString file_path = obj["file_path"].toString ();
                    qDebug()<<"收到了文件"<<"file_path:"+file_path;
                }
            }
        }else if(array_name=="face"){
            qDebug()<<"表情消息,"<<"表情代码："<<array[1].toInt ();
        }else{
            qDebug()<<"其他类型的数据："<<array_name;
        }
    }else if(temp2.isString ()){
        qDebug()<<"消息内容是："+temp2.toString ();
    }
}

void QQCommand::disposeInputNotify(QJsonObject &obj)
{
    QString from_uin = doubleToString (obj, "from_uin");

    qDebug()<<"是输入状态的消息";
    qDebug()<<getValue (from_uin+"nick", from_uin).toString ()+"正在输入";
}

void QQCommand::disposeFriendStatusChanged(QJsonObject &obj)
{
    QString uin = doubleToString (obj, "uin");
    QString status = obj["status"].toString ();
    int client_type = obj["client_type"].toInt ();
    
    qDebug()<<"是好友状态改变的信息"<<getValue (uin+"nick", uin).toString ()<<"状态改变为"<<status<<"客户端类型:"<<client_type;
}

void QQCommand::disposeFriendMessage(QJsonObject &obj)
{
    qDebug()<<"是聊天消息";
    
    QString from_uin = doubleToString (obj, "from_uin");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");

    /*qDebug()<<"\n消息发送者的uin是："+from_uin;
    qDebug()<<"消息id1是："+msg_id;
    qDebug()<<"消息id2是："+msg_id2;
    qDebug()<<"消息类型是："+msg_type;
    qDebug()<<"回复的ip地址是："+reply_ip;
    qDebug()<<"接收方的uin是："+to_uin;*/
    qDebug()<<"来自"+getValue (from_uin+"nick", from_uin).toString ()+"的消息";
    
    analysisMessage (obj);
}

void QQCommand::disposeGroupMessage(QJsonObject &obj)
{
    qDebug()<<"是群消息";
    
    QString from_uin = doubleToString (obj, "from_uin");;
    QString group_code = doubleToString (obj, "group_code");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");
    QString send_uin = doubleToString (obj, "send_uin");

    /*qDebug()<<"\n群的uin是："+from_uin;
    qDebug()<<"群code是: "+group_code;
    qDebug()<<"消息id1是："+msg_id;
    qDebug()<<"消息id2是："+msg_id2;
    qDebug()<<"消息类型是："+msg_type;
    qDebug()<<"回复的ip地址是："+reply_ip;
    qDebug()<<"接收方的uin是："+to_uin;*/
    qDebug()<<"来自"+getValue (from_uin+"nick", from_uin).toString ()+"中"+getValue (send_uin+"nick", send_uin).toString ()+"的消息";
    
    analysisMessage (obj);
}

void QQCommand::disposeDiscuMessage(QJsonObject &obj)
{
    qDebug()<<"是讨论组消息";
    
    QString from_uin = doubleToString (obj, "from_uin");;
    QString did = doubleToString (obj, "did");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");
    QString send_uin = doubleToString (obj, "send_uin");

    /*qDebug()<<"\n群的uin是："+from_uin;
    qDebug()<<"讨论组did是: "+did;
    qDebug()<<"消息id1是："+msg_id;
    qDebug()<<"消息id2是："+msg_id2;
    qDebug()<<"消息类型是："+msg_type;
    qDebug()<<"回复的ip地址是："+reply_ip;
    qDebug()<<"接收方的uin是："+to_uin;*/
    qDebug()<<"来自"+getValue (did+"nick", did).toString ()+"中"+getValue (send_uin+"nick", send_uin).toString ()+"的消息";
    
    analysisMessage (obj);
}

QString QQCommand::doubleToString(QJsonObject &obj, QString name)
{
    if(!obj.isEmpty ()){
        QJsonValue temp = obj[name];
        if(temp.isDouble ())
            return QString::number ((quint64)obj[name].toDouble ());
    }
    return name;
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
    QQmlComponent component(Utility::createUtilityClass ()->qmlEngine (), "./qml/MyMessageBox.qml");
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

void QQCommand::updataApi(const QString content)
{
    qDebug()<<content;
}

QString QQCommand::getHash()
{
    QJSValueList list;
    list<<QJSValue(userQQ())<<QJSValue(Utility::createUtilityClass ()->getCookie ("ptwebqq"));
    return jsEngine.globalObject ().property ("getHash").call (list).toString ();
}

QString QQCommand::encryptionPassword(const QString &uin, const QString &code)
{
    QJSValueList list;
    list<<QJSValue(userPassword())<<QJSValue(uin)<<QJSValue(code);
    return jsEngine.globalObject ().property ("encryptionPassword").call (list).toString ();
}

int QQCommand::openMessageBox(QJSValue value)
{
    MyMessageBox message;
    message.setStyleSource (QUrl::fromLocalFile ("style/messageBoxStyle.css"));
    QJSValue temp = value.property ("icon");
    if( !temp.isUndefined () ){
        message.setIcon ((MyMessageBox::Icon)temp.toInt ());
    }
    temp = value.property ("detailedText");
    if( !temp.isUndefined () ) {
        message.setDetailedText (temp.toString ());
    }
    temp = value.property ("standardButtons");
    if( !temp.isUndefined () ) {
        message.setStandardButtons ((MyMessageBox::StandardButtons)temp.toInt ());
    }
    temp = value.property ("text");
    if( !temp.isUndefined () ) {
        message.setText (temp.toString ());
    }
    temp = value.property ("iconPixmap");
    if( !temp.isUndefined () ) {
        message.setIconPixmap (QPixmap(temp.toString ()));
    }
    temp = value.property ("textFormat");
    if( !temp.isUndefined () ) {
        message.setTextFormat ((Qt::TextFormat)temp.toInt ());
    }
    temp = value.property ("informativeText");
    if( !temp.isUndefined () ) {
        message.setInformativeText (temp.toString ());
    }
    temp = value.property ("textInteractionFlags");
    if( !temp.isUndefined () ) {
        message.setTextInteractionFlags ((Qt::TextInteractionFlags)temp.toInt ());
    }
    return message.exec ();
}
