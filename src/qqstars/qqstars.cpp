#include "qqstars.h"
#include "utility.h"
#include <QJsonDocument>
#include <QSettings>
#include "mywindow.h"

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
    
    request = new QNetworkRequest;
    request->setUrl (QUrl("http://d.web2.qq.com/channel/poll2"));
    request->setRawHeader ("Origin", "http://d.web2.qq.com");
    request->setRawHeader ("Accept", "*/*");
    request->setRawHeader ("Referer", "http://d.web2.qq.com/proxy.html?v=20110331002&callback=1&id=2");
    request->setRawHeader ("Content-Type", "application/x-www-form-urlencoded");
    request->setRawHeader ("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/29.0.1547.66 Safari/537.36 LBBROWSER");
    
    manager = new NetworkAccessManager(this);
    connect (manager, SIGNAL(finished(QNetworkReply*)), SLOT(poll2Finished(QNetworkReply*)));
    
    jsEngine = new QJSEngine();//此对象用来加载js文件（为qq提供api）
    loadApi ();//加载api的js文件
}

QString QQCommand::userStatusToString() const
{
    return m_userStatusToString;
}

QQCommand::LoginStatus QQCommand::loginStatus() const
{
    return m_loginStatus;
}

QString QQCommand::userPassword() const
{
    return m_userPassword;
}

double QQCommand::windowScale() const
{
    return m_windowScale;
}

QString QQCommand::userQQ() const
{
    return m_userQQ;
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
    manager->post (*request, poll2_data);
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
                                disposeFriendMessage (obj, InputNotify);//解析好友正在输入的消息
                            }else if( poll_type=="buddies_status_change" ){
                                disposeFriendStatusChanged(obj);//好友状态改变信息
                            }else if( poll_type=="group_message" ){
                                disposeGroupMessage (obj);//解析群消息
                            }else if( poll_type=="discu_message" ){
                                disposeDiscuMessage (obj);//解析讨论组消息
                            }else if( poll_type=="file_message" ){
                                //qDebug()<<"发送文件消息";
                                disposeFriendMessage (obj, FileMessage);
                            }else if( poll_type=="av_request" ){
                                //qDebug()<<"视频聊天消息";
                                disposeFriendMessage (obj, AvRequest);
                            }else if( poll_type=="av_refuse" ){
                                //qDebug()<<"取消开视频";
                                disposeFriendMessage (obj, AvRefuse);
                            }else if( poll_type=="shake_message" ){
                                //qDebug()<<"窗口抖动消息";
                                disposeFriendMessage (obj, ShakeWindow);//解析窗口抖动消息
                            }else if( poll_type=="system_message" ){
                                disposeSystemMessage (obj);//解析系统消息
                            }else if( poll_type=="sys_g_msg" ){
                                disposeSystemMessage (obj);//解析系统消息
                            }else if(poll_type == "sess_message"){
                                //disposeStrangerMessage (obj);//解析陌生人的消息
                                disposeFriendMessage (obj);//解析好友的普通消息
                            }else{
                                qDebug()<<"其他消息"<<poll_type;
                            }
                        }
                        beginPoll2();
                    }else if(retcode==102){
                        beginPoll2();
                    }else{
                        qDebug()<<"qq已掉线，即将重新登录";
                        QMetaObject::invokeMethod (this, "reLogin");//调用槽reLogin（在qml中定义）
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

void QQCommand::onChatMainWindowClose()//如果主聊天窗口关闭，那就销毁所有已经建立的聊天页面
{
    foreach (QQuickItem *item, map_chatPage) {
        if(item!=NULL){
            QString key = map_chatPage.key (item);
            QString uin,typeStr;
            for(int i=0;i<key.length ();++i){
                if(key[i].isNumber ()){
                    uin = key.mid (i);
                    typeStr = key.mid (0,i);
                    break;
                }
            }
            QQItemInfo* item_info = createQQItemInfo (uin, typeStr);
            if(item_info!=NULL){
                item_info->startClearChatRecordsTimer ();//启动清空聊天记录的定时器
            }
            item->deleteLater ();//销毁此页面
        }
    }
    map_chatPage.clear ();//清空所有对象
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
    jsEngine->evaluate(contents, fileName);
}

QString QQCommand::disposeMessage(QJsonObject &obj)
{
    QString result;
    FontStyle font_style;
    QJsonArray content = obj["content"].toArray ();
    QJsonValue temp2 = content[0];
    if(temp2.isArray ()){
        QJsonArray font = temp2.toArray ();
        foreach (QJsonValue temp3, font) {
            if(temp3.isObject ()){
                obj = temp3.toObject ();
                font_style.size = obj["size"].toInt ();
                font_style.color = obj["color"].toString ();
                QJsonArray style = obj["style"].toArray ();
                font_style.bold = (bool)style[0].toInt ();//加黑
                font_style.italic = (bool)style[1].toInt ();//斜体
                font_style.underline = (bool)style[2].toInt ();//下划线
                font_style.family = obj["name"].toString ();
            }
        }
    }
    for( int i=1;i<content.size ();++i ){
        temp2 = content[i];
        if(temp2.isArray ()){
            QJsonArray array = temp2.toArray ();
            QString array_name = array[0].toString ();
            if(array_name=="cface"){//为表情消息
                foreach (QJsonValue temp3, array) {
                    if(temp3.isObject ()){
                        obj = temp3.toObject ();
                        result.append (textToHtml (font_style, "[[此处为表情]]"));//添加纯文本消息
                        //QString file_id = doubleToString (obj, "file_id");
                        //QString key = obj["key"].toString ();
                        //QString name = obj["name"].toString ();
                        //QString server = obj["server"].toString ();
                        //qDebug()<<"收到了文件"<<"file_id:"+file_id<<"key:"+key<<"name:"+name<<"server:"+server;
                        //data.append (QString("{")+"\"type\":"+QString::number (Image)+",\"file_id\":\""+file_id+"\",\"name\":\""+name+"\",\"key\":\""+key+"\",\"server\":\""+server+"\"},");
                    }
                }
            }else if(array_name=="offpic"){//为图片消息
                foreach (QJsonValue temp3, array) {
                    if(temp3.isObject ()){
                        obj = temp3.toObject ();
                        result.append (textToHtml (font_style, "[[此处为图片]]"));//添加纯文本消息
                        //QString file_path = obj["file_path"].toString ();
                        //qDebug()<<"收到了文件"<<"file_path:"+file_path;
                        //data.append (QString("{")+"\"type\":"+QString::number (Image)+",\"file_path\":\""+file_path+"\"},");
                    }
                }
            }else if(array_name=="face"){//为表情消息
                int faceNumber = array[1].toInt ();//转化为int
                result.append (textToHtml (font_style, "[[为表情:"+QString::number (faceNumber)+"]]"));//添加纯文本消息
                //qDebug()<<"表情消息,"<<"表情代码："<<array[1].toInt ();
                //data.append (QString("{")+"\"type\":"+QString::number (Face)+",\"face_code\":"+QString::number (array[1].toInt ())+"},");
            }else{
                qDebug()<<"其他类型的数据："<<array_name;
            }
        }else if(temp2.isString ()&&temp2.toString ()!=""){//否则为纯文本消息
            result.append (textToHtml (font_style, temp2.toString ()));//添加纯文本消息
            //qDebug()<<"消息内容是："+temp2.toString ();
            //data.append (QString("{")+"\"type\":"+QString::number (Text)+",\"text\":\""+temp2.toString ()+"\"},");
        }
    }
    return result;
}

void QQCommand::disposeFriendStatusChanged(QJsonObject &obj)
{
    QString uin = doubleToString (obj, "uin");
    QString status = obj["status"].toString ();
    //int client_type = obj["client_type"].toInt ();
    
    emit friendStatusChanged (uin, status);
    //qDebug()<<"是好友状态改变的信息"<<getValue (uin+"nick", uin).toString ()<<"状态改变为"<<status<<"客户端类型:"<<client_type;
    //emit messageArrive (SystemMessage, uin, "{\"type\":"+QString::number (FriendStatusChanged)+",\"status\":\""+status+"\"}");
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
        QString data = disposeMessage (obj);//先处理基本消息
        QQItemInfo *info = createQQItemInfo (from_uin, QQItemInfo::Friend);
        ChatMessageInfo *message_info = new ChatMessageInfo;
        message_info->setSenderUin (from_uin);
        message_info->setContentData (data);
        message_info->setDate (QDate::currentDate ());
        message_info->setTime (QTime::currentTime ());
        info->addChatRecord (message_info);//给from_uin的info对象增加聊天记录
        if(!isChatPageExist(from_uin, QQItemInfo::Friend)){//如果聊天页面不存在
            info->setUnreadMessagesCount (info->unreadMessagesCount ()+1);
            //增加未读消息的个数
        }
        //qDebug()<<"收到了好友消息："<<data;
        emit newMessage (from_uin, (int)QQItemInfo::Friend, message_info);
        break;
    }
    case InputNotify:
        emit friendInputNotify (from_uin);//发送好友正在输入的信号
        //emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (InputNotify)+"}]}");
        break;
    case FileMessage:{
        /*QString mode = obj["mode"].toString ();
        if( mode=="recv" ){
            QString file_name = obj["name"].toString ();
            emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (FileMessage)+",\"flag\":1,\"name\":\""+file_name+"\"}]}");
        }else{
            emit messageArrive (Friend, from_uin, "{\"content\":[{\"type\":"+QString::number (FileMessage)+",\"flag\":0}]}");
        }*/
        break;
    }
    case AvRequest:
        break;
    case AvRefuse:
        break;
    case ShakeWindow://如果是窗口抖动的消息
        addChatPage (from_uin, QQItemInfo::Friend);//现将此聊天窗口显示出来
        emit shakeWindow (from_uin);//然后发送信号告诉窗口有人抖动他
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
        QString data = disposeMessage (obj);
        QQItemInfo *info = createQQItemInfo (from_uin, QQItemInfo::Group);
        ChatMessageInfo *message_info = new ChatMessageInfo;
        message_info->setSenderUin (send_uin);
        message_info->setContentData (data);
        message_info->setDate (QDate::currentDate ());
        message_info->setTime (QTime::currentTime ());
        info->addChatRecord (message_info);//给from_uin的info对象增加聊天记录
        if(!isChatPageExist(from_uin, QQItemInfo::Group)){//如果聊天页面不存在
            info->setUnreadMessagesCount (info->unreadMessagesCount ()+1);
            //增加未读消息的个数
        }
        emit newMessage (from_uin, (int)QQItemInfo::Group, message_info);
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
        QString data = disposeMessage (obj);
        QQItemInfo *info = createQQItemInfo (did, QQItemInfo::Discu);
        ChatMessageInfo *message_info = new ChatMessageInfo;
        message_info->setSenderUin (send_uin);
        message_info->setContentData (data);
        message_info->setDate (QDate::currentDate ());
        message_info->setTime (QTime::currentTime ());
        info->addChatRecord (message_info);//给from_uin的info对象增加聊天记录
        if(!isChatPageExist(did, QQItemInfo::Discu)){//如果聊天页面不存在
            info->setUnreadMessagesCount (info->unreadMessagesCount ()+1);
            //增加未读消息的个数
        }
        emit newMessage (did, (int)QQItemInfo::Discu, message_info);
        break;
    }
    default:
        break;
    }
}

void QQCommand::disposeStrangerMessage(QJsonObject &, QQCommand::MessageType )
{
    /*QString from_uin = doubleToString (obj, "from_uin");
    QString msg_id = doubleToString (obj, "msg_id");
    QString msg_id2 = doubleToString (obj, "msg_id2");
    QString msg_type = doubleToString (obj, "msg_type");
    QString reply_ip = doubleToString (obj, "reply_ip");
    QString to_uin = doubleToString (obj, "to_uin");*/
    
}

void QQCommand::disposeSystemMessage(QJsonObject &obj)
{
    QString type = obj["type"].toString ();
    if(type == "verify_required"){//好友验证信息
        QString account = doubleToString (obj, "account");
        QString from_uin = doubleToString (obj, "from_uin");
        //emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (FriendVerify)+",\"account\"\":"+account+"\"}");
    }else if(type == "group_admin_op"){//管理员变动信息
        QString from_uin = doubleToString (obj, "from_uin");
        QString uin = doubleToString (obj, "uin");
        QString uin_flag = doubleToString (obj, "uin_flag");
        //emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (GroupAdmin)+",\"uin\":\""+uin+"\",\"flag\":\""+uin_flag+"\"}");
    }else if(type == "group_leave"){//群成员变动信息
        QString from_uin = doubleToString (obj, "from_uin");
        QString old_member = doubleToString (obj, "old_member");
        //emit messageArrive (SystemMessage, from_uin, "{\"type\":"+QString::number (GroupLeave)+",\"old_member\":\""+old_member+"\"}");
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

QString QQCommand::doubleToString(QJsonObject &obj, const QString &name)
{
    if(!obj.isEmpty ()){
        QJsonValue temp = obj[name];
        if(temp.isDouble ())
            return QString::number ((quint64)obj[name].toDouble ());
    }
    return name;
}

QString QQCommand::textToHtml(QQCommand::FontStyle &style, QString data)
{
    data.replace("&","&amp;");     
    data.replace(">","&gt;");
    data.replace("<","&lt;");
    data.replace("\"","&quot;");
    data.replace("\'","&#39;");
    data.replace(" ","&nbsp;");
    data.replace("\n","<br>");
    data.replace("\r","<br>");
    //上面这几行代码的顺序不能乱，否则会造成多次替换
    
    QString result="<font";
    if(style.size>0)
        result.append (" size=\""+QString::number (style.size/3)+"\"");
    if(style.color!=""){
        if(style.color[0].isNumber ())
            result.append (" color=\"#"+style.color+"\"");
        else
            result.append (" color=\""+style.color+"\"");
    }
    if(style.family!="")
        result.append (" face=\""+style.family+"\"");
    result.append (">");
    if(style.bold)
        result.append ("<b>");
    if(style.underline)
        result.append ("<u>");
    if(style.italic)
        result.append ("<i>");
    result.append (data);//把文本包含进去
    if(style.italic)
        result.append ("</i>");
    if(style.underline)
        result.append ("</u>");
    if(style.bold)
        result.append ("</b>");
    result.append ("</font>");
    
    return result;
}

QQItemInfo *QQCommand::createQQItemInfo(const QString& uin, const QString& typeString)
{
    //QThread::msleep (10);//线程休眠10毫秒
    if(uin=="")
        return NULL;
    QString name = typeString+uin;
    if(map_itemInfo.value (name, NULL)){
        QQItemInfo* info = qobject_cast<QQItemInfo*>(map_itemInfo[name]);
        return info;
    }
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    QQmlComponent component(engine, "./qml/QQItemInfo/"+typeString+"Info.qml");
    QQItemInfo* info = qobject_cast<QQItemInfo*>(component.create ());
    if(info!=NULL){
        map_itemInfo[name] = info;
        info->setParent (this);
        info->setUserQQ (userQQ());
        info->setUin (uin);
    }
    return info;
}

void QQCommand::setLoginStatus(QQCommand::LoginStatus arg)
{
    if (m_loginStatus != arg) {
        m_loginStatus = arg;
        emit loginStatusChanged();
    }
}

void QQCommand::startPoll2(const QByteArray &data)
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
        FriendInfo::setAccount (arg);
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
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    if(warning_info_window){
        warning_info_window->show ();
    }else{
        QQmlComponent component(engine, "./qml/Utility/MyMessageBox.qml");
        QObject *obj = component.create ();
        warning_info_window = qobject_cast<MyWindow*>(obj);
        if(obj)
            obj->setProperty ("text", QVariant(message));
        else
            qDebug()<<"创建MyMessageBox.qml失败";
    }
}

void QQCommand::downloadImage(int senderType, QUrl url, QString account, QString imageSize, QJSValue callbackFun)
{
    QString path = QQItemInfo::localCachePath ((QQItemInfo::QQItemType)senderType, userQQ(), account);
    //先获取此qq为account，类型为senderType的缓存目录，将此目录传给下载图片的函数。此图片下载完成就会存入此路径
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

FriendInfo* QQCommand::createFriendInfo(const QString uin)
{
    FriendInfo* info = qobject_cast<FriendInfo*>(createQQItemInfo(uin, QQItemInfo::Friend));
    return info;
}

GroupInfo* QQCommand::createGroupInfo(const QString uin)
{
    GroupInfo* info = qobject_cast<GroupInfo*>(createQQItemInfo(uin, QQItemInfo::Group));
    return info;
}

DiscuInfo* QQCommand::createDiscuInfo(const QString uin)
{
    DiscuInfo* info = qobject_cast<DiscuInfo*>(createQQItemInfo(uin, QQItemInfo::Discu));
    return info;
}

ChatMessageInfo *QQCommand::createChatMessageInfo(const QString senderUin, const QString data)
{
    ChatMessageInfo* info = new ChatMessageInfo;
    info->setSenderUin (senderUin);
    info->setContentData (data);
    info->setDate (QDate::currentDate ());
    info->setTime (QTime::currentTime ());
    return info;
}

void QQCommand::addChatPage(QString uin, int senderType)
{
    if(uin=="")
        return;
    QString typeStr = QQItemInfo::typeToString ((QQItemInfo::QQItemType)senderType);//获取此类型的字符串表达形式
    
    if(map_chatPage.contains(typeStr+uin)){//如果已经存在
        emit activeChatPageChanged (map_chatPage[typeStr+uin]);//活跃的聊天Page改变为temp
        mainChatWindowCommand->show ();//显示出聊天窗口
        return;//如果已经处在此Page就返回
    }
    
    QQmlEngine *engine = Utility::createUtilityClass ()->qmlEngine ();
    if(mainChatWindowCommand.isNull ()){
        QQmlComponent component(engine, "./qml/Chat/ChatWindowCommand.qml");
        QObject *temp_obj = component.create ();
        //qDebug()<<"创建窗口是否出错："<<component.errorString ();
        mainChatWindowCommand = qobject_cast<MyWindow*>(temp_obj);
        if(mainChatWindowCommand){
            connect (mainChatWindowCommand.data (), &MyWindow::closeing, this, &QQCommand::onChatMainWindowClose);
            //链接信号和槽，为聊天主窗口关闭时销毁对象所用
            foreach (QQuickItem *item, mainChatWindowCommand->contentItem ()->childItems ()) {
                if(item->objectName () == "ChatWindowCommandItem"){
                    mainChatWindowCommand_item = item;//将聊天页面的父对象储存起来
                    break;
                }
            }
        }else{
            qDebug()<<"创建ChatWindowCommand.qml出错";
            return;//如果出错就返回
        }
    }

    QString qmlName = "./qml/Chat/"+typeStr+"ChatPage.qml";
    QQmlComponent component(engine, qmlName);
    QQuickItem *item = qobject_cast<QQuickItem*>(component.create ());//新建聊天页面
    if(item){
        item->setParentItem (mainChatWindowCommand_item);//设置聊天页面的父对象
        item->setProperty ("myuin", uin);//设置他的uin
        item->setProperty ("type", senderType);//设置他的类型
        map_chatPage[typeStr+uin] = item;//储存聊天页面
        QQItemInfo* item_info = createQQItemInfo (uin, (QQItemInfo::QQItemType)senderType);
        if(item_info!=NULL){
            item_info->stopClearChatRecordsTimer ();//停止清空聊天记录的定时器
        }
        emit addChatPageToWindow (item);//发送信号告知qml
    }else{
        qDebug()<<"创建"+qmlName+"出错";
    }
    mainChatWindowCommand->show ();//显示出聊天窗口
}

void QQCommand::removeChatPage(QString uin, int senderType)
{
    QQItemInfo::QQItemType type = (QQItemInfo::QQItemType)senderType;
    QString typeStr = QQItemInfo::typeToString (type);//获取此类型的字符串表达形式
    QQuickItem *item = map_chatPage.value (typeStr+uin, NULL);
    if(item!=NULL){
        item->deleteLater ();//销毁此对象
        map_chatPage.remove (typeStr+uin);//如果对象已经为空则移除此对象
        QQItemInfo* item_info = createQQItemInfo (uin, type);
        if(item_info!=NULL){
            item_info->startClearChatRecordsTimer ();//启动清空聊天记录的定时器
            qDebug()<<"启动了销毁聊天记录的定时器";
        }
    }else{
        qDebug()<<typeStr+uin<<"page已经为NULL";
    }
    //qDebug()<<item;
    foreach (QQuickItem *temp, map_chatPage) {//改变当前活跃页面为首先找到的第一个不为空的chatPage
        if(temp){
            //qDebug()<<temp;
            emit activeChatPageChanged (temp);
            break;
        }else{
            QString key = map_chatPage.key (temp);
            map_chatPage.remove (key);//如果对象已经为空则移除此对象
            qDebug()<<key+"为NULL，已被销毁";
        }
    }
}

QVariant QQCommand::value(const QString &key, const QVariant &defaultValue) const
{
    return mysettings->value (key, defaultValue);
}

void QQCommand::setValue(const QString &key, const QVariant &value)
{
    mysettings->setValue (key, value);
}

void QQCommand::shakeChatMainWindow(QQuickItem *item)
{
    emit activeChatPageChanged (item);
    if(QMetaObject::invokeMethod (mainChatWindowCommand, "windowShake")){
        qDebug()<<"窗口抖动成功";
    }else{
        qDebug()<<"窗口抖动失败";
    }
}

void QQCommand::openSqlDatabase()
{
    FriendInfo::openSqlDatabase (userQQ());//打开数据库
}

/*void QQCommand::saveAlias(int type, QString uin, QString alias)
{
    QString name = QQItemInfo::typeToString ((QQItemInfo::QQItemType)type)+uin;
    map_alias[name] = alias;
}*/

void QQCommand::updataApi(const QString& content)
{
    qDebug()<<"更新api.js"<<content;
}

QQItemInfo *QQCommand::createQQItemInfo(const QString& uin, QQItemInfo::QQItemType type)
{
    QString typeString = QQItemInfo::typeToString (type);
    return createQQItemInfo (uin, typeString);
}

bool QQCommand::isChatPageExist(const QString& uin, int senderType)
{
    QString typeStr = QQItemInfo::typeToString ((QQItemInfo::QQItemType)senderType);//获取此类型的字符串表达形式
    return map_chatPage.contains(typeStr+uin);
}

void QQCommand::addFriendUin(const QString &uin)
{
    friendsUin.append (uin+" ");
    //qDebug()<<"增加好友uin:"+uin<<friendsUin;
}

bool QQCommand::isStranger(const QString &uin)
{
    return friendsUin.indexOf (uin)<0;
}

QString QQCommand::getHash()
{
    QJSValueList list;
    list<<QJSValue(userQQ())<<QJSValue(Utility::createUtilityClass ()->getCookie ("ptwebqq"));
    return jsEngine->globalObject ().property ("getHash").call (list).toString ();
}

QString QQCommand::encryptionPassword(const QString &uin, const QString &code)
{
    QJSValueList list;
    list<<QJSValue(userPassword())<<QJSValue(uin)<<QJSValue(code);
    return jsEngine->globalObject ().property ("encryptionPassword").call (list).toString ();
}

QVariant QQCommand::getLoginedQQInfo()
{
    Utility *utility = Utility::createUtilityClass ();
    QByteArray reply="[";
    QString qqs = utility->value ("qq_account", "").toString ();
    QStringList qq_list = qqs.split (",");
    foreach (QString qq, qq_list) {
        if(qq!=""){
            QStringList temp = qq.split (".");
            if(temp.size ()==2){//如果有两个，一个为qq号，一个为昵称
                FriendInfo info;
                QString account = temp[0];
                info.setUserQQ (account);
                info.setAccount (account);
                reply.append ("{\"account\":\""+account
                              +"\",\"nick\":\""+QByteArray::fromHex (temp[1].toUtf8 ())
                        +"\",\"avatarSource\":\""+info.avatar240 ()+"\"},");
            }
        }
    }
    reply.replace (reply.size ()-1,1,"]");
    return QVariant(QJsonDocument::fromJson (reply).array ());
}

void QQCommand::removeLoginedQQInfo(const QString account, bool rmLocalCache)
{
    Utility *utility = Utility::createUtilityClass ();
    QString qqs = utility->value ("qq_account", "").toString ();
    QStringList qq_list = qqs.split (",");
    foreach (QString qq, qq_list) {
        if(qq!=""){
            QStringList temp = qq.split (".");
            if(temp.size ()==2){//如果有两个，一个为qq号，一个为昵称
                if(temp[0]==account){//如果查找到此qq
                    qqs.replace (qq+",", "");//替换掉
                    utility->setValue ("qq_account", qqs);//替换掉原来的值
                    FriendInfo info;
                    info.setUserQQ (account);
                    info.setAccount (account);
                    info.clearSettings ();//清除配置内容
                    if(rmLocalCache){//如果要删除本地缓存
                        utility->removePath (info.localCachePath ());
                    }
                    return;
                }
            }
        }
    }
}

void QQCommand::addLoginedQQInfo(const QString account, const QString nick)
{
    Utility *utility = Utility::createUtilityClass ();
    QString qqs = utility->value ("qq_account", "").toString ();
    QString addStr = account+"."+nick.toUtf8 ().toHex ()+",";
    if(qqs.indexOf (addStr)<0){//如果这条信息不存在
        qqs.insert (0, addStr);
        utility->setValue ("qq_account", qqs);//添加进去
    }
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
