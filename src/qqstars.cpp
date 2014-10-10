#include "qqstars.h"
#include "utility.h"
#include <QJsonDocument>
#include <QSettings>
#include "aes.h"

QQCommand::QQCommand(QQuickItem *parent) :
    FriendInfo(parent)
{
    connect (this, &QQItemInfo::settingsChanged, this, &QQCommand::rememberPasswordChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQCommand::autoLoginChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQCommand::userStatusChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQCommand::initUserPassword);
    connect (this, &QQCommand::userStatusChanged, this, &QQCommand::setStatusToString);
    
    Utility *utility=Utility::createUtilityClass ();
    int temp1 = utility->value ("proxyType", QNetworkProxy::NoProxy).toInt ();
    QString temp2 = utility->value ("proxyLocation", "").toString ();
    QString temp3 = utility->value ("proxyPort", "").toString ();
    QString temp4 = utility->value ("proxyUsername", "").toString ();
    QString temp5 = utility->value ("proxyPassword", "").toString ();
    utility->setApplicationProxy (temp1, temp2, temp3, temp4, temp5);
    
    setUserQQ (utility->value ("mainqq","").toString ());
    m_loginStatus = Offline;
    m_windowScale = 1;
    
    request.setUrl (QUrl("http://d.web2.qq.com/channel/poll2"));
    request.setRawHeader ("Origin", "http://d.web2.qq.com");
    request.setRawHeader ("Accept", "*/*");
    request.setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    request.setRawHeader ("Content-Type", "application/x-www-form-urlencoded");
    request.setRawHeader ("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36 LBBROWSER");
    
    connect (&manager, SIGNAL(finished(QNetworkReply*)), SLOT(poll2Finished(QNetworkReply*)));
    
    loadApi ();//加载api的js文件
}

bool QQCommand::rememberPassword() const
{
    if(isCanUseSetting())
        return mysettings->value ("rememberPassword", false).toBool ();
    return false;
}

bool QQCommand::autoLogin() const
{
    if(isCanUseSetting())
        return mysettings->value ("autoLogin", false).toBool ();
    return false;
}

QString QQCommand::codeText() const
{
    if(code_window){
        return code_window->property ("code").toString ();
    }
    return "";
}

void QQCommand::setStatusToString()
{
    switch(userStatus ())
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
    emit userStatusToStringChanged ();
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
                                disposeFriendMessage (obj);//解析好友的普通消息
                            }else if( poll_type=="input_notify" ){
                                disposeFriendMessage (obj, InputNotify);
                                //disposeInputNotify (obj);//解析好友正在输入的消息
                            }else if( poll_type=="buddies_status_change" ){
                                disposeFriendStatusChanged(obj);//好友状态改变信息
                            }else if( poll_type=="group_message" ){
                                disposeGroupMessage (obj);//解析群消息
                            }else if( poll_type=="discu_message" ){
                                disposeDiscuMessage (obj);//解析讨论组消息
                            }else if( poll_type=="file_message" ){
                                //qDebug()<<"发送文件消息";
                                disposeFriendMessage (obj, FileMessage);
                                //disposeFileMessage(obj);//解析文件传输的消息
                            }else if( poll_type=="av_request" ){
                                //qDebug()<<"视频聊天消息";
                                disposeFriendMessage (obj, AvRequest);
                                //disposeAvMessage(obj, true);
                            }else if( poll_type=="av_refuse" ){
                                //qDebug()<<"取消开视频";
                                disposeFriendMessage (obj, AvRefuse);
                                //disposeAvMessage(obj, false);
                            }else if( poll_type=="shake_message" ){
                                //qDebug()<<"窗口抖动消息";
                                disposeFriendMessage (obj, ShakeWindow);//解析窗口抖动消息
                                //disposeShakeMessage(obj);
                            }else if( poll_type=="system_message" ){
                                disposeSystemMessage (obj);//解析系统消息
                            }else if( poll_type=="sys_g_msg" ){
                                disposeSystemMessage (obj);//解析系统消息
                            }else if(poll_type == "sess_message"){
                                disposeStrangerMessage (obj);//解析陌生人的消息
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
    }else{
        beginPoll2();//重新post
    }
}

void QQCommand::initUserPassword()
{
    if(isCanUseSetting()){
        QString pass=mysettings->value ("password", "").toString ();
        setUserPassword (Utility::createUtilityClass ()->stringUncrypt (pass, "xingchenQQ"));
    }
}

QQCommand::QQStatus QQCommand::userStatus()
{
    if(isCanUseSetting())
        return (QQStatus)mysettings->value ("QQStatus", (int)Online).toInt ();
    return Online;
}

void QQCommand::setUserStatus(QQCommand::QQStatus new_status)
{
    if( isCanUseSetting()&&userStatus()!=qq_status ) {
        mysettings->setValue ("QQStatus", (int)new_status);
        emit userStatusChanged ();
    }
}

void QQCommand::loadApi()
{
    QString fileName = "qml/Api/api.js";
    QFile scriptFile(fileName);
    if (!scriptFile.open(QIODevice::ReadOnly))
        qDebug()<<"打开"+fileName+"失败";
    QString contents = scriptFile.readAll ();
    scriptFile.close();
    jsEngine.evaluate(contents, fileName);
}

QString QQCommand::analysisMessage(QJsonObject &obj)
{
    QString result="{";
    QString data="\"content\":[";
    
    QJsonArray content = obj["content"].toArray ();
    QJsonValue temp2 = content[0];
    if(temp2.isArray ()){
        QJsonArray font = temp2.toArray ();
        foreach (QJsonValue temp3, font) {
            if(temp3.isObject ()){
                obj = temp3.toObject ();
                /*int size = obj["size"].toInt ();
                QString color = obj["color"].toString ();
                QJsonArray style = obj["style"].toArray ();
                bool bold = (bool)style[0].toInt ();//加黑
                bool italic = (bool)style[1].toInt ();//斜体
                bool underline = (bool)style[2].toInt ();//下划线
                QString name = obj["name"].toString ();*/
                
                QJsonDocument font_doc(obj);
                result.append ("\"font\":"+font_doc.toJson ());
                /*qDebug()<<"所使用字体是："+name;
                qDebug()<<"字体大小是："+size;
                qDebug()<<"字体颜色是："+color;
                qDebug()<<"黑体："<<bold;
                qDebug()<<"斜体："<<italic;
                qDebug()<<"下划线："<<underline;*/
            }
        }
    }
    for( int i=1;i<content.size ();++i ){
        temp2 = content[i];
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
                        //qDebug()<<"收到了文件"<<"file_id:"+file_id<<"key:"+key<<"name:"+name<<"server:"+server;
                        data.append (QString("{")+"\"type\":"+QString::number (Image)+",\"file_id\":\""+file_id+"\",\"name\":\""+name+"\",\"key\":\""+key+"\",\"server\":\""+server+"\"},");
                    }
                }
            }else if(array_name=="offpic"){
                foreach (QJsonValue temp3, array) {
                    if(temp3.isObject ()){
                        obj = temp3.toObject ();
                        QString file_path = obj["file_path"].toString ();
                        //qDebug()<<"收到了文件"<<"file_path:"+file_path;
                        data.append (QString("{")+"\"type\":"+QString::number (Image)+",\"file_path\":\""+file_path+"\"},");
                    }
                }
            }else if(array_name=="face"){
                //qDebug()<<"表情消息,"<<"表情代码："<<array[1].toInt ();
                data.append (QString("{")+"\"type\":"+QString::number (Face)+",\"face_code\":"+QString::number (array[1].toInt ())+"},");
            }else{
                qDebug()<<"其他类型的数据："<<array_name;
            }
        }else if(temp2.isString ()&&temp2.toString ()!=""){
            //qDebug()<<"消息内容是："+temp2.toString ();
            data.append (QString("{")+"\"type\":"+QString::number (Text)+",\"text\":\""+temp2.toString ()+"\"},");
        }
    }
    result.append (","+data.mid (0,data.size ()-1)+"]}");
    return result;
}

/*void QQCommand::disposeInputNotify(QJsonObject &obj)
{
    QString from_uin = doubleToString (obj, "from_uin");
    //qDebug()<<"是输入状态的消息";
    //qDebug()<<getValue (from_uin+"nick", from_uin).toString ()+"正在输入";
    emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (InputNotify)+"}]}");
}*/

void QQCommand::disposeFriendStatusChanged(QJsonObject &obj)
{
    QString uin = doubleToString (obj, "uin");
    QString status = obj["status"].toString ();
    int client_type = obj["client_type"].toInt ();
    //qDebug()<<"是好友状态改变的信息"<<getValue (uin+"nick", uin).toString ()<<"状态改变为"<<status<<"客户端类型:"<<client_type;
    
    emit messageArrive (SystemMessage, uin, "{\"type\":"+QString::number (FriendStatusChanged)+",\"status\":\""+status+"\"}");
}

void QQCommand::disposeFriendMessage(QJsonObject &obj, QQCommand::MessageType type)
{
    //qDebug()<<"是聊天消息";
    QString from_uin = doubleToString (obj, "from_uin");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");

    switch (type) 
    {
    case GeneralMessage:{
        QString data = analysisMessage (obj);
        emit messageArrive (Friend, from_uin, data);
        break;
    }
    case InputNotify:
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (InputNotify)+"}]}");
        break;
    case FileMessage:{
        QString mode = obj["mode"].toString ();
        if( mode=="recv" ){
            QString file_name = obj["name"].toString ();
            emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (FileMessage)+",\"flag\":1,\"name\":\""+file_name+"\"}]}");
        }else{
            emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (FileMessage)+",\"flag\":0}]}");
        }
        break;
    }
    case AvRequest:
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (AvRequest)+"}]}");
        break;
    case AvRefuse:
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (AvRefuse)+"}]}");
        break;
    case ShakeWindow:
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (ShakeWindow)+"}]}");
        break;
    default:
        break;
    }
}

void QQCommand::disposeGroupMessage(QJsonObject &obj, QQCommand::MessageType type)
{
    //qDebug()<<"是群消息";
    
    QString from_uin = doubleToString (obj, "from_uin");;
    QString group_code = doubleToString (obj, "group_code");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");
    QString send_uin = doubleToString (obj, "send_uin");

    switch (type) {
    case GeneralMessage:{
        QString temp = analysisMessage (obj);
        temp.insert (1, "\"send_uin\":"+send_uin+",");
        emit messageArrive (Group, from_uin, temp);
        break;
    }
    default:
        break;
    }
    
}

void QQCommand::disposeDiscuMessage(QJsonObject &obj, QQCommand::MessageType type)
{
    //qDebug()<<"是讨论组消息";
    
    QString from_uin = doubleToString (obj, "from_uin");;
    QString did = doubleToString (obj, "did");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");
    QString send_uin = doubleToString (obj, "send_uin");
    
    switch (type) {
    case GeneralMessage:{
        QString temp = analysisMessage (obj);
        temp.insert (1, "\"send_uin\":"+send_uin+",");
        emit messageArrive (Discu, did, temp);
        break;
    }
    default:
        break;
    }
}

void QQCommand::disposeStrangerMessage(QJsonObject &obj, QQCommand::MessageType type)
{
    QString from_uin = doubleToString (obj, "from_uin");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");
    
}

void QQCommand::disposeSystemMessage(QJsonObject &obj)
{
    QString type = obj["type"].toString ();
    if(type == "verify_required"){//好友验证信息
        QString account = doubleToString (obj, "account");
        QString from_uin = doubleToString (obj, "from_uin");
        emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (FriendVerify)+",\"account\":"+account+"}");
    }else if(type == "group_admin_op"){//管理员变动信息
        QString from_uin = doubleToString (obj, "from_uin");
        QString uin = doubleToString (obj, "uin");
        QString uin_flag = doubleToString (obj, "uin_flag");
        emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (GroupAdmin)+",\"uin\":"+uin+",\"flag\":"+uin_flag+"}");
    }else if(type == "group_leave"){//群成员变动信息
        QString from_uin = doubleToString (obj, "from_uin");
        QString old_member = doubleToString (obj, "old_member");
        emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (GroupLeave)+",\"old_member\":"+old_member+"}");
    }else{//其他系统消息
        qDebug()<<"其他系统消息:"<<type;
    }
}

/*void QQCommand::disposeFileMessage(QJsonObject &obj)
{
    QString from_uin = doubleToString (obj, "from_uin");
    QString mode = obj["mode"].toString ();
    if( mode=="recv" ){
        QString file_name = obj["name"].toString ();
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (SendFile)+"}, \"name\":\""+file_name+"\"]}");
    }else{
        emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (SendFile)+"}]}");
    }
}

void QQCommand::disposeAvMessage(QJsonObject &obj, bool open)
{
    QString from_uin = doubleToString (obj, "from_uin");
    emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (open?AvRequest:AvRefuse)+"}]}");
}

void QQCommand::disposeShakeMessage(QJsonObject &obj)
{
    QString from_uin = doubleToString (obj, "from_uin");
    emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (ShakeWindow)+"}]}");
}*/

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
        emit loginStatusChanged();
    }
}

void QQCommand::startPoll2(QByteArray data)
{
    poll2_data = data;
    //poll2_timer.start ();
    beginPoll2();
}

void QQCommand::setUserQQ(QString arg)
{
    if (m_userQQ != arg) {
        m_userQQ = arg;
        FriendInfo::setUserQQ (arg);
        FriendInfo::setUin (arg);
        emit userQQChanged();
    }
}

void QQCommand::setUserPassword(QString arg)
{
    if (m_userPassword != arg) {
        //qDebug()<<"设置了密码"<<arg;
        m_userPassword = arg;
        emit userPasswordChanged();
    }
}

void QQCommand::showWarningInfo(QString message)
{
    if(warning_info_window){
        warning_info_window->show ();
    }else{
        QQmlComponent component(new QQmlEngine, "./qml/Utility/MyMessageBox.qml");
        QObject *obj = component.create ();
        warning_info_window = qobject_cast<MyWindow*>(obj);
        if(obj)
            obj->setProperty ("text", QVariant(message));
        else
            qDebug()<<"创建MyMessageBox.qml失败";
    }
}

void QQCommand::downloadImage(QUrl url, QString uin, QString imageSize, QJSValue callbackFun)
{
    QString path = QDir::homePath ()+"/webqq/"+userQQ ()+"/"+uin;
    Utility::createUtilityClass ()->downloadImage (callbackFun, url, path, "avatar-"+imageSize);
}

void QQCommand::showCodeWindow(const QJSValue callbackFun, const QString code_uin)
{
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    if(!code_window){
        QQmlComponent component(engine, "./qml/Utility/CodeInput.qml");
        QObject *obj = component.create ();
        if(obj){
            code_window = qobject_cast<MyWindow*>(obj);
        }else{
            qDebug()<<"创建CodeInput.qml失败";
            return;
        }
    }
    //qDebug()<<"显示验证码"<<code_uin<<code_window;
    if(code_window){
        QJSValue value = engine->newQObject (code_window);
        if(value.isObject ())
            value.setProperty ("backFun", callbackFun);
        QString url = "https://ssl.captcha.qq.com/getimage?aid=1003903&r=0.9101365606766194&uin="+userQQ()+"&cap_cd="+code_uin;
        code_window->setProperty ("source", url);
        code_window->show ();
    }
}

void QQCommand::closeCodeWindow()
{
    if(code_window){
        code_window->close ();
        code_window->deleteLater ();
    }
}

void QQCommand::updataCode()
{
    if(code_window){
        QMetaObject::invokeMethod (code_window, "updateCode");//调用刷新验证码
    }
}

FriendInfo *QQCommand::createFriendInfo(const QString uin)
{
    if(uin=="")
        return NULL;
    if(uin == userQQ())//如果这个好友就是自己的话
        return this;
    
    QString name = "friend"+uin;
    if(map_itemInfo.value (name, NULL)){
        FriendInfo* info = qobject_cast<FriendInfo*>(map_itemInfo[name]);
        return info;
    }
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    QQmlComponent component(engine, "./qml/QQItemInfo/FriendInfo.qml");
    FriendInfo *info = qobject_cast<FriendInfo*>(component.create ());
    info->setUserQQ (userQQ());
    info->setUin (uin);
    map_itemInfo[name] = info;
    return info;
}

GroupInfo *QQCommand::createGroupInfo(const QString uin)
{
    if(uin=="")
        return NULL;
    QString name = "group"+uin;
    if(map_itemInfo.value (name, NULL)){
        GroupInfo* info = qobject_cast<GroupInfo*>(map_itemInfo[name]);
        return info;
    }
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    QQmlComponent component(engine, "./qml/QQItemInfo/GroupInfo.qml");
    GroupInfo *info = qobject_cast<GroupInfo*>(component.create ());
    info->setUserQQ (userQQ());
    info->setUin (uin);
    map_itemInfo[name] = info;
    return info;
}

DiscuInfo *QQCommand::createDiscuInfo(const QString uin)
{
    if(uin=="")
        return NULL;
    QString name = "discu"+uin;
    if(map_itemInfo.value (name, NULL)){
        DiscuInfo* info = qobject_cast<DiscuInfo*>(map_itemInfo[name]);
        return info;
    }
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    QQmlComponent component(engine, "./qml/QQItemInfo/DiscuInfo.qml");
    DiscuInfo *info = qobject_cast<DiscuInfo*>(component.create ());
    info->setUserQQ (userQQ());
    info->setUin (uin);
    map_itemInfo[name] = info;
    return info;
}

RecentInfo *QQCommand::createRecentInfo(QQItemType type, const QString uin)
{
    RecentInfo *info = NULL;
    switch (type) {
    case QQItemInfo::Friend:
        info = new RecentInfo(createFriendInfo (uin));
        break;
    case QQItemInfo::Group:
        info = new RecentInfo(createGroupInfo (uin));
        break;
    case QQItemInfo::Discu:
        info = new RecentInfo(createDiscuInfo (uin));
        break;
    default:
        break;
    }
    return info;
}

/*void QQCommand::setValue(const QString &key, const QVariant &value, const QString & userQQ)
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    mysettings.setValue (key, value);
}

QVariant QQCommand::value(const QString &key, const QVariant &defaultValue, const QString & userQQ ) const
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    return mysettings.value (key, defaultValue);
}

void QQCommand::removeValue(const QString &key, const QString & userQQ )
{
    QSettings mysettings(QDir::homePath ()+"/webqq/"+(userQQ==""?m_userQQ:userQQ)+"/.config.ini", QSettings::IniFormat);
    mysettings.remove (key);
}*/

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

void QQCommand::setWindowScale(double arg)
{
    if (m_windowScale != arg) {
        m_windowScale = arg;
        emit windowScaleChanged();
    }
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

void QQCommand::setRememberPassword(bool arg)
{
    if (mysettings&&rememberPassword ()!= arg) {
        mysettings->setValue ("rememberPassword", arg);
        if(!arg)
            mysettings->remove ("password");
        emit rememberPasswordChanged();
    }
}

void QQCommand::setAutoLogin(bool arg)
{
    if (mysettings&&autoLogin() != arg) {
        mysettings->setValue ("autoLogin", arg);
        emit autoLoginChanged();
    }
}

void QQCommand::saveUserPassword()
{
    if(rememberPassword()&&mysettings){//先判断是否记住了密码
        QString pass = Utility::createUtilityClass ()->stringEncrypt (userPassword (), "xingchenQQ");
        mysettings->setValue ("password", pass);
    }
}


QQItemInfo::QQItemInfo(QQItemType type, QQuickItem *parent):
    QQuickItem(parent), typeString(type)
{
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::accountChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::nickChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::aliasChanged);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar40Changed);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar240Changed);
    connect (this, &QQItemInfo::nickChanged, this, &QQItemInfo::updataAliasOrNick);
    connect (this, &QQItemInfo::aliasChanged, this, &QQItemInfo::updataAliasOrNick);
    
    switch (type) {
    case Friend:
        typeString = "friend";
        break;
    case Group:
        typeString = "group";
    case Discu:
        typeString = "discu";
    case Recent:
        typeString = "recent";
    default:
        break;
    }
    mysettings = new QSettings(QDir::homePath ()+"/webqq/"+m_userQQ+"/"+typeString+"_"+m_uin+"/.config.ini", QSettings::IniFormat);
}

void QQItemInfo::initSettings()
{
    QString userqq = userQQ ();
    QString uin = this->uin();
    
    if(uin==""||userqq=="")
        return;
    QString name = QDir::homePath ()+"/webqq/"+userqq+"/"+typeString+"_"+uin+"/.config.ini";
    if(mysettings){
        if(mysettings->fileName ()==name)
            return;
        mysettings->deleteLater ();
    }
    mysettings = new QSettings(name, QSettings::IniFormat);
    
    emit settingsChanged ();
}

bool QQItemInfo::isCanUseSetting() const
{
    return (mysettings&&userQQ()!=""&&uin()!="");
}

QString QQItemInfo::uin() const
{
    return m_uin;
}

QString QQItemInfo::nick() const
{
    if(isCanUseSetting())
        return mysettings->value (typeString+"_"+uin()+"nick", "").toString ();
    return "";
}

QString QQItemInfo::alias() const
{
    return m_alias;
}


QString QQItemInfo::avatar40() const
{
    if(isCanUseSetting())
        return mysettings->value (typeString+"_"+uin()+"avatar-40", "qrc:/images/avatar.png").toString ();
    return "qrc:/images/avatar.png";
}

QString QQItemInfo::avatar240() const
{
    if(isCanUseSetting())
        return mysettings->value (typeString+"_"+uin()+"avatar-240", "qrc:/images/avatar.png").toString ();
    return "qrc:/images/avatar.png";
}

QString QQItemInfo::aliasOrNick()
{
    QString m_alias = alias();
    if(m_alias!="")
        return m_alias;
    return nick ();
}

QString QQItemInfo::userQQ() const
{
    return m_userQQ;
}

QString QQItemInfo::typeToString()
{
    return typeString;
}

QString QQItemInfo::account() const
{
    if(isCanUseSetting())
        return mysettings->value (typeString+"_"+uin()+"account", "").toString ();
    return "";
}

void QQItemInfo::setUin(QString arg)
{
    if (m_uin != arg) {//m_uin==""保证uin只被设置一次
        m_uin = arg;
        initSettings();
        emit uinChanged ();
    }
}

void QQItemInfo::setNick(QString arg)
{
    if (isCanUseSetting()&&nick() != arg) {
        mysettings->setValue (typeString+"_"+uin()+"nick", arg);
        emit nickChanged ();
    }
}

void QQItemInfo::setAlias(QString arg)
{
    if(m_alias!=arg){
        m_alias = arg;
        emit aliasChanged();
    }
}

void QQItemInfo::setAccount(QString arg)
{
    if (isCanUseSetting()&&account() != arg) {
        mysettings->setValue (typeString+"_"+uin()+"account", arg);
        emit accountChanged();
    }
}

void QQItemInfo::setAvatar40(QString arg)
{
    if (isCanUseSetting()&&avatar40() != arg) {
        mysettings->setValue (typeString+"_"+uin()+"avatar-40", arg);
        emit avatar40Changed();
    }
}

void QQItemInfo::setAvatar240(QString arg)
{
    if (isCanUseSetting()&&avatar240() != arg) {
        mysettings->setValue (typeString+"_"+uin()+"avatar-240", arg);
        emit avatar240Changed();
    }
}

void QQItemInfo::updataAliasOrNick()
{
    QString arg = aliasOrNick ();
    if (m_aliasOrNick != arg) {
        m_aliasOrNick = arg;
        emit aliasOrNickChanged();
    }
}

void QQItemInfo::setUserQQ(QString arg)
{
    if(m_userQQ!=arg) {
        m_userQQ = arg;
        initSettings();
        emit userQQChanged ();
    }
}

void QQItemInfo::clearSettings()
{
    if(isCanUseSetting())
        mysettings->clear ();//清除所有储存的信息
}

FriendInfo::FriendInfo(QQuickItem *parent):
    QQItemInfo(Friend, parent)
{
    connect (this, &QQItemInfo::settingsChanged, this, &FriendInfo::qQSignatureChanged);
}

QString FriendInfo::QQSignature()
{
    if(isCanUseSetting())
        return mysettings->value (typeString+"_"+uin()+"signature", "").toString ();
    return "";
}

void FriendInfo::setQQSignature(QString arg)
{
    if (isCanUseSetting()&&QQSignature() != arg) {
        mysettings->setValue (typeString+"_"+uin()+"signature", arg);
        emit qQSignatureChanged();
    }
}


GroupInfo::GroupInfo(QQuickItem *parent):
    QQItemInfo(Group, parent)
{
    
}


DiscuInfo::DiscuInfo(QQuickItem *parent):
    QQItemInfo(Discu, parent)
{
    
}


RecentInfo::RecentInfo(FriendInfo *info, QQuickItem *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToFriend (info);
}

RecentInfo::RecentInfo(GroupInfo *info, QQuickItem *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToGroup (info);
}

RecentInfo::RecentInfo(DiscuInfo *info, QQuickItem *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToDiscu (info);
}

QQuickItem *RecentInfo::infoData() const
{
    return m_infoData;
}

FriendInfo *RecentInfo::infoToFriend() const
{
    return m_infoToFriend;
}

GroupInfo *RecentInfo::infoToGroup() const
{
    return m_infoToGroup;
}

DiscuInfo *RecentInfo::infoToDiscu() const
{
    return m_infoToDiscu;
}

void RecentInfo::setInfoToFriend(FriendInfo *arg)
{
    if (m_infoToFriend != arg) {
        m_infoToFriend = arg;
        emit infoToFriendChanged(arg);
    }
}

void RecentInfo::setInfoToGroup(GroupInfo *arg)
{
    if (m_infoToGroup != arg) {
        m_infoToGroup = arg;
        emit infoToGroupChanged(arg);
    }
}

void RecentInfo::setInfoToDiscu(DiscuInfo *arg)
{
    if (m_infoToDiscu != arg) {
        m_infoToDiscu = arg;
        emit infoToDiscuChanged(arg);
    }
}

void RecentInfo::setInfoData(QQuickItem *info)
{
    m_infoData = info;
    emit infoDataChanged ();
}
