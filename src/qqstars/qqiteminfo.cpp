#include "qqiteminfo.h"
#include "utility.h"

QString ChatMessageInfo::senderUin() const
{
    return m_senderUin;
}

QString ChatMessageInfo::contentData() const
{
    return m_contentData;
}

QDate ChatMessageInfo::date() const
{
    return m_date;
}

QTime ChatMessageInfo::time() const
{
    return m_time;
}

void ChatMessageInfo::setSenderUin(QString arg)
{
    if (m_senderUin == arg)
        return;
    
    m_senderUin = arg;
    emit senderUinChanged(arg);
}

void ChatMessageInfo::setContentData(QString arg)
{
    if (m_contentData == arg)
        return;
    
    m_contentData = arg;
    emit contentDataChanged(arg);
}

void ChatMessageInfo::setDate(QDate arg)
{
    if (m_date == arg)
        return;
    
    m_date = arg;
    emit dateChanged(arg);
}

void ChatMessageInfo::setTime(QTime arg)
{
    if (m_time == arg)
        return;
    
    m_time = arg;
    emit timeChanged(arg);
}

QSqlDatabase QQItemInfoPrivate::sqlite_db = QSqlDatabase::addDatabase ("QSQLITE");
QQItemInfoPrivate::QQItemInfoPrivate(QQuickItem *):
    QQuickItem(0)
{
    connect (this, &QQItemInfoPrivate::sql_open, this, &QQItemInfoPrivate::m_openSqlDatabase);
    connect (this, &QQItemInfoPrivate::sql_close, this, &QQItemInfoPrivate::m_closeSqlDatabase);
    connect (this, &QQItemInfoPrivate::sql_insert, this, &QQItemInfoPrivate::m_insertData);
    connect (this, &QQItemInfoPrivate::sql_insertDatas, this, &QQItemInfoPrivate::m_insertDatas);
    connect (this, &QQItemInfoPrivate::sql_getDatas, this, &QQItemInfoPrivate::m_getDatas);
    moveToThread (&thread);
    thread.start ();//启动线程
}

QQItemInfoPrivate::~QQItemInfoPrivate()
{
    sqlite_db.close();//关闭数据库
    thread.quit ();
    thread.wait ();
}

bool QQItemInfoPrivate::tableAvailable(const QString &tableName)
{
    if(tableName!=""&&sqlite_db.isOpen ()){//如果数据库已经打开
        QString temp = "create table if not exists "+tableName+
                "(index INT,senderUin VARCHAR[16],message TEXT,mydate DATE,mytime TIME)";
        //创建一个表，如果这表不存在，表的列为uin message mydate mytime
        QSqlQuery query = sqlite_db.exec (temp);
        if(query.lastError ().type ()==QSqlError::NoError){//如果上面的语句执行没有出错
            return true;
        }else{
            qDebug()<<"执行"<<temp<<"出错："<<query.lastError ().text ();
        }
    }else{
        qDebug()<<"数据库未打开";
    }
    return false;
}

void QQItemInfoPrivate::m_openSqlDatabase(const QString &userqq)
{
    if(!sqlite_db.isOpen ()){//如果数据库未打开
        //sqlite_db = QSqlDatabase::addDatabase("QSQLITE");
        sqlite_db.setHostName ("localhost");
        QString name = QDir::homePath ()+"/webqq/"+userqq+"/.QQData.db";
        sqlite_db.setDatabaseName (name);
        sqlite_db.setUserName ("雨后星辰");
        sqlite_db.setPassword ("XingChenQQ");
        if(!sqlite_db.open ()){
            qDebug()<<"数据库 "<<name<<" 打开失败";
        }
    }
}

void QQItemInfoPrivate::m_closeSqlDatabase()
{
    if(!sqlite_db.isOpen ()){
        sqlite_db.close ();
    }
}

void QQItemInfoPrivate::m_insertData(const QString& tableName, ChatMessageInfo *data)
{
    if(tableAvailable (tableName)){//判断表是否可以操作
        QString temp = "insert into "+tableName+
                " values(:index,:senderUin,:message,:mydate,:mytime)";
        QSqlQuery insert_query;
        insert_query.prepare (temp);
        int index = sqlite_db.exec ("select count(*) from "+tableName).size ()+1;//设置此条数据的索引值
        Utility *utility = Utility::createUtilityClass ();
        insert_query.bindValue (":index", index);//设置数据的索引值（为此条数据的唯一标识）
        insert_query.bindValue (":senderUin", data->senderUin ());
        insert_query.bindValue (":message", utility->stringEncrypt (data->contentData (), "XingchenQQ123"));
        //将contentData加密后储存
        insert_query.bindValue (":mydate", data->date());
        insert_query.bindValue (":mytime", data->time());
        if(insert_query.exec ()){//如果上面的语句执行没有出错
            qDebug()<<"插入数据成功";
        }else{
            qDebug()<<"执行"<<temp<<"出错："<<insert_query.lastError ().text ();
        }
    }
}

void QQItemInfoPrivate::m_insertDatas(const QString &tableName, ChatMessageInfoList *datas)
{
    if(tableAvailable (tableName)){//判断表是否可以操作
        sqlite_db.transaction ();//开启事务操作
        for (int i=0;i<datas->size ();++i) {
            ChatMessageInfo* data = datas->at (i);
            if(data!=NULL){
                QString temp = "insert into "+tableName+
                        " values(:index,:senderUin,:message,:mydate,:mytime)";
                QSqlQuery insert_query;
                insert_query.prepare (temp);
                int index = sqlite_db.exec ("select count(*) from "+tableName).size ()+1;//设置此条数据的索引值
                Utility *utility = Utility::createUtilityClass ();
                insert_query.bindValue (":index", index);//设置数据的索引值（为此条数据的唯一标识）
                insert_query.bindValue (":senderUin", data->senderUin ());
                insert_query.bindValue (":message", utility->stringEncrypt (data->contentData (), "XingchenQQ123"));
                insert_query.bindValue (":mydate", data->date());
                insert_query.bindValue (":mytime", data->time());
                insert_query.exec ();
            }
        }
        if(sqlite_db.commit ()){//提交事务操作,如果上面的语句执行没有出错
            qDebug()<<"插入多条数据成功";
        }else{
            qDebug()<<"执行多条插入出错："<<sqlite_db.lastError ().text ();
        }
    }
}

void QQItemInfoPrivate::m_getDatas(const QString &tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList* datas)
{
    if(datas!=NULL&&tableAvailable (tableName)){//判断表是否可以操作
        QString sql_code = "select index from "+tableName
                +"where senderUin="+currentData->senderUin ()
                +" and message="+currentData->contentData ()
                +" and mydate="+currentData->date().toString ()
                +" and mytime="+currentData->time().toString ();
        QSqlQuery sql_query = sqlite_db.exec (sql_code);
        if(sql_query.lastError ().type ()==QSqlError::NoError){//如果查询没有出错
            if(sql_query.size ()>0){
                int currentIndex = sql_query.value (0).toInt ();//当前数据的索引为
                sql_code = "select * from "+tableName
                        +"where index<"+QString::number (currentIndex)
                        +" and index<="+QString::number (currentIndex+count);
                sql_query.exec (sql_code);
                if(sql_query.lastError ().type ()==QSqlError::NoError){//如果查询没有出错
                    qDebug()<<"查询多条数据完成，数据的个数："<<sql_query.size ();
                    while(sql_query.next ()){
                        ChatMessageInfo *data = new ChatMessageInfo;
                        Utility *utility = Utility::createUtilityClass ();
                        data->setSenderUin (sql_query.value (1).toString ());//从第一个开始，因为0为index
                        data->setContentData (utility->stringUncrypt (sql_query.value (2).toString (), "XingchenQQ123"));
                        //取回聊天内容时要解密
                        data->setDate (QDate::fromString (sql_query.value (3).toString ()));
                        data->setTime (QTime::fromString (sql_query.value (4).toString ()));
                        datas->append (data);//将查询到的结果添加到列表中
                    }
                    emit getDatasFinished (datas);//发送信号，告知数据获取完成
                }else{
                    qDebug()<<"执行"<<sql_code<<"出错："<<sql_query.lastError ().text ();
                }
            }else{
                qDebug()<<"执行"<<sql_code<<"未查询到结果";
            }
        }else{
            qDebug()<<"执行"<<sql_code<<"出错："<<sql_query.lastError ().text ();
        }
    }
}

void QQItemInfoPrivate::openSqlDatabase(const QString& userqq)
{
    emit sql_open (userqq);//发送信号打开数据库
}

void QQItemInfoPrivate::closeSqlDatabase()
{
    emit sql_close ();//关闭数据库
}

void QQItemInfoPrivate::insertData(const QString& tableName, ChatMessageInfo *data)
{
    emit sql_insert (tableName, data);
}

void QQItemInfoPrivate::insertDatas(const QString &tableName, ChatMessageInfoList* datas)
{
    emit sql_insertDatas (tableName, datas);
}

void QQItemInfoPrivate::getDatas(const QString &tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList* datas)
{
    emit sql_getDatas (tableName, count, currentData, datas);
}

QQItemInfo::QQItemInfo(QQItemInfoPrivate::QQItemType type, QQuickItem *parent):
    QQuickItem(parent), m_mytype (type)
{
    m_uin = "";
    m_account = "";
    m_aliasOrNick = "";
    m_userQQ = "";
    m_nick = "";
    m_alias = "";
    
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar40Changed);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar240Changed);
    connect (this, &QQItemInfo::nickChanged, this, &QQItemInfo::updataAliasOrNick);
    connect (this, &QQItemInfo::aliasChanged, this, &QQItemInfo::updataAliasOrNick);
    
    typeString = typeToString (type);
}

void QQItemInfo::initSettings()
{
    QString userqq = userQQ ();
    QString account = this->account ();
    
    if(account==""||userqq=="")
        return;
    QString name = QDir::homePath ()+"/webqq/"+userqq+"/"+typeString+"_"+account+"/.config.ini";
    //qDebug()<<"设置了QSettings为"<<name;
    if(mysettings){
        //qDebug()<<mysettings->fileName ();
        if(mysettings->fileName ()==name)
            return;
        mysettings->deleteLater ();
    }
    mysettings = new QSettings(name, QSettings::IniFormat);
    emit settingsChanged ();
}

bool QQItemInfo::isCanUseSetting() const
{
    return (mytype()!=QQItemInfoPrivate::Discu&&userQQ()!=""&&account()!=""&&mysettings);
}

QString QQItemInfo::uin() const
{
    return m_uin;
}

QString QQItemInfo::nick() const
{
    return m_nick;
}

QString QQItemInfo::alias() const
{
    return m_alias;
}


QString QQItemInfo::avatar40() const
{
    if(isCanUseSetting())
        return mysettings->value ("avatar-40", "qrc:/images/avatar.png").toString ();
    return "qrc:/images/avatar.png";
}

QString QQItemInfo::avatar240() const
{
    if(isCanUseSetting())
        return mysettings->value ("avatar-240", "qrc:/images/avatar.png").toString ();
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

const QString QQItemInfo::typeToString(QQItemInfoPrivate::QQItemType type)
{
    switch (type) {
    case QQItemInfoPrivate::Friend:
        return "friend";
        break;
    case QQItemInfoPrivate::Group:
        return "group";
    case QQItemInfoPrivate::Discu:
        return "discu";
    default:
        return "";
    }
}

const QString QQItemInfo::localCachePath(QQItemInfoPrivate::QQItemType type, const QString &userqq, const QString &account)
{
    QString typeString = typeToString (type);
    return QDir::homePath ()+"/webqq/"+userqq+"/"+typeString+"_"+account;
}

QQItemInfoPrivate::QQItemType QQItemInfo::mytype() const
{
    return m_mytype;
}

QString QQItemInfo::account() const
{
    return m_account;
}

void QQItemInfo::setUin(QString arg)
{
    if (m_uin != arg) {
        m_uin = arg;
        if(mytype ()==QQItemInfoPrivate::Discu)//如果是讨论组
            setAccount (arg);//讨论组无真实qq，所以用uin充当
        emit uinChanged ();
    }
}

void QQItemInfo::setNick(QString arg)
{
    if (m_nick != arg) {
        m_nick = arg;
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
    if (m_account != arg) {
        m_account = arg;
        initSettings();
        emit accountChanged();
    }
}

void QQItemInfo::setAvatar40(QString arg)
{
    if(isCanUseSetting ()){
        mysettings->setValue ("avatar-40", arg);
        emit avatar40Changed();
    }
}

void QQItemInfo::setAvatar240(QString arg)
{
    if(isCanUseSetting ()){
        mysettings->setValue ("avatar-240", arg);
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
    if(isCanUseSetting()){
        mysettings->clear ();//清除所有储存的信息
        qDebug()<<mysettings->fileName ()<<"清除成功";
    }
}

const QString QQItemInfo::localCachePath()
{
    return QDir::homePath ()+"/webqq/"+userQQ()+"/"+typeString+"_"+account();
}

ChatMessageInfo *QQItemInfo::addChatRecords(const QString senderUin, const QString html)
{
    if(senderUin!=""&&html!=""){
        ChatMessageInfo *data = new ChatMessageInfo();
        data->setContentData (html);//将内容加密后储存
        data->setSenderUin (senderUin);
        data->setDate (QDate::currentDate ());//当前日期
        data->setTime (QTime::currentTime ());//当前时间
        queue_chatRecords.append (data);//将此条记录加到队列当中
        return data;
    }else{
        return NULL;
    }
}

FriendInfo::FriendInfo(QQuickItem *parent):
    QQItemInfo(QQItemInfoPrivate::Friend, parent)
{
    m_signature = "";
    connect (this, &QQItemInfo::settingsChanged, this, &FriendInfo::onSettingsChanged);
    //链接信号，处理settings对象改变的信号
    getChatRecordsing=false;//记录现在是否在请求获取聊天记录
    connect (&itemInfoPrivate, SIGNAL(getDatasFinished(ChatMessageInfoList*)), SIGNAL(getLocalChatRecordsFinished(ChatMessageInfoList*)));
    //链接信号，处理从数据库中读取聊天记录后的操作
}

QString FriendInfo::QQSignature()
{
    return m_signature;
}

void FriendInfo::onSettingsChanged()
{
    if(QQSignature ()==""){//如果个性签名为空
        QString temp = mysettings->value ("signature", "").toString ();
        if(temp!=""){
            setQQSignature (temp);
        }else{//如果获得的个性签名为空，就发送信号在qml端去网络获取个性签名
            emit httpGetQQSignature();//发送信号
        }
    }
}

void FriendInfo::setQQSignature(QString arg)
{
    if (m_signature != arg) {
        m_signature = arg;
        if(isCanUseSetting ())//如果可以储存配置信息
            mysettings->setValue ("signature", arg);
        emit qQSignatureChanged();
    }
}

void FriendInfo::openSqlDatabase(const QString &userqq)
{
    itemInfoPrivate.openSqlDatabase (userqq);
}

void FriendInfo::closeSqlDatabase()
{
    itemInfoPrivate.closeSqlDatabase ();
}

void FriendInfo::saveChatMessageToLocal()
{
    if(account ()!=""){//qq账户（qq号码）一定不能为空，因为它是消息发送者的唯一标识
        QString tableName = "table_"+typeToString ()+account();
        itemInfoPrivate.insertDatas (tableName, &queue_chatRecords);//将此条消息记录插入到数据库当中
    }
}

void FriendInfo::getLocalChatRecords(ChatMessageInfo *currentData, int count)
{
    if(account ()!=""&&!getChatRecordsing){//qq账户（qq号码）一定不能为空，因为它是消息发送者的唯一标识
        QString tableName = "table_"+typeToString ()+account();
        getChatRecordsing = true;//将此值置为true
        itemInfoPrivate.getDatas (tableName, count, currentData, new ChatMessageInfoList());
        //开始获取聊天记录
    }
}

GroupInfo::GroupInfo(QQuickItem *parent):
    QQItemInfo(QQItemInfoPrivate::Group, parent)
{
    m_code = "";
}

QString GroupInfo::code() const
{
    return m_code;
}

void GroupInfo::setCode(QString arg)
{
    if (m_code == arg)
        return;
    
    m_code = arg;
    emit codeChanged(arg);
}


DiscuInfo::DiscuInfo(QQuickItem *parent):
    QQItemInfo(QQItemInfoPrivate::Discu, parent)
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

ChatMessageInfo *ChatMessageInfoList::at(int i)
{
    return list.at (i);
}

int ChatMessageInfoList::length()
{
    return list.length ();
}

int ChatMessageInfoList::size()
{
    return length();
}

void ChatMessageInfoList::append(ChatMessageInfo *obj)
{
    list.append (obj);
}

void ChatMessageInfoList::destroy()
{
    this->deleteLater ();
}
