#include "qqiteminfo.h"
#include "utility.h"
#include "qqstars.h"

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

int ChatMessageInfo::messageId() const
{
    return m_messageId;
}

int ChatMessageInfo::messageId2() const
{
    return m_messageId2;
}

const QQItemInfo *ChatMessageInfo::getParent()
{
    return qobject_cast<QQItemInfo *>(parent());
}

ChatMessageInfo::ChatMessageInfo(QQItemInfo *parent)
{
    ChatMessageInfo(-1, parent);
}

ChatMessageInfo::ChatMessageInfo(int messageID, QQItemInfo *parent):
    QObject(parent)
{
    m_messageId=messageID;
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

void ChatMessageInfo::setMessageId2(int arg)
{
    if (m_messageId2 != arg) {
        m_messageId2 = arg;
        emit messageId2Changed(arg);
    }
}

QSqlDatabase DatabaseOperation::sqlite_db;
DatabaseOperation *DatabaseOperation::createDatabaseOperation()
{
    static DatabaseOperation me;
    return &me;
}

DatabaseOperation::DatabaseOperation():
    QObject(0)
{
    if (!sqlite_db.isValid()){//如果数据库无效，则进行初始化
        sqlite_db = QSqlDatabase::addDatabase ("QSQLITE");
    }
}

DatabaseOperation::~DatabaseOperation()
{
    closeSqlDatabase();//关闭数据库
}

bool DatabaseOperation::tableAvailable(const QString &tableName)
{
    if(tableName!=""&&sqlite_db.isOpen ()){//如果数据库已经打开
        QString temp = "create table if not exists "+tableName+
                "(myindex INTEGER,senderUin VARCHAR[16],message TEXT,mydate DATE,mytime TIME)";
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

bool DatabaseOperation::openSqlDatabase(const QString& userqq)
{
    //emit sql_open (userqq);//发送信号打开数据库
    if(!sqlite_db.isOpen ()){//如果数据库未打开
        //sqlite_db = QSqlDatabase::addDatabase("QSQLITE");
        sqlite_db.setHostName ("localhost");
        QString name = QDir::homePath ()+"/.webqq/"+userqq+"/.QQData.db";
        sqlite_db.setDatabaseName (name);
        sqlite_db.setUserName ("雨后星辰");
        sqlite_db.setPassword ("XingChenQQ");
        
        return sqlite_db.open ();
    }
    return true;
}

void DatabaseOperation::closeSqlDatabase()
{
    sqlite_db.close ();
}

void DatabaseOperation::insertData(const QString& tableName, ChatMessageInfo *data)
{
    if(tableAvailable (tableName)){//判断表是否可以操作
        QString temp = "insert into "+tableName+
                " values(:myindex,:senderUin,:message,:mydate,:mytime)";
        QSqlQuery insert_query;
        insert_query.prepare (temp);
        int index = sqlite_db.exec ("select count(*) from "+tableName).size ()+1;//设置此条数据的索引值
        Utility *utility = Utility::createUtilityClass ();
        insert_query.bindValue (":myindex", index);//设置数据的索引值（为此条数据的唯一标识）
        insert_query.bindValue (":senderUin", data->senderUin ());
        insert_query.bindValue (":message", utility->stringEncrypt (data->contentData (), "XingchenQQ123"));
        //将contentData加密后储存
        insert_query.bindValue (":mydate", data->date());
        insert_query.bindValue (":mytime", data->time());
        if(insert_query.exec ()){//如果上面的语句执行没有出错
            //qDebug()<<"插入数据成功";
        }else{
            qDebug()<<"执行"<<temp<<"出错："<<insert_query.lastError ().text ();
        }
    }
}

void DatabaseOperation::insertDatas(const QString &tableName, ChatMessageInfoList* datas)
{
    //emit sql_insertDatas (tableName, datas);
    if(tableAvailable (tableName)){//判断表是否可以操作
        sqlite_db.transaction ();//开启事务操作
        for (int i=0;i<datas->size ();++i) {
            ChatMessageInfo* data = datas->at (i);
            if(data!=NULL){
                insertData (tableName, data);
            }
        }
        if(sqlite_db.commit ()){//提交事务操作,如果上面的语句执行没有出错
            qDebug()<<"插入"+QString::number (datas->size ())+"条数据成功";
        }else{
            qDebug()<<"执行多条插入出错："<<sqlite_db.lastError ().text ();
        }
    }
}

void DatabaseOperation::getDatas(const QString &tableName, int count, ChatMessageInfo *currentData, ChatMessageInfoList* datas)
{
    //emit sql_getDatas (tableName, count, currentData, datas);
    if(tableAvailable (tableName)){//判断表是否可以操作
        QString sql_code = "select myindex from "+tableName
                +"where senderUin="+currentData->senderUin ()
                +" and message="+currentData->contentData ()
                +" and mydate="+currentData->date().toString ()
                +" and mytime="+currentData->time().toString ();
        QSqlQuery sql_query = sqlite_db.exec (sql_code);
        if(sql_query.lastError ().type ()==QSqlError::NoError){//如果查询没有出错
            if(sql_query.size ()>0){
                int currentIndex = sql_query.value (0).toInt ();//当前数据的索引为
                sql_code = "select * from "+tableName
                        +"where myindex<"+QString::number (currentIndex)
                        +" and myindex<="+QString::number (currentIndex+count);
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
                    //emit getDatasFinished (datas);//发送信号，告知数据获取完成
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

QQItemInfo::QQItemInfo(QQItemInfo::QQItemType type, QObject *parent):
    QObject(parent), m_mytype (type)
{
    m_uin = "";
    m_account = "";
    m_aliasOrNick = "";
    m_userQQ = "";
    m_nick = "";
    m_alias = "";
    m_unreadMessagesCount = 0;
    messageID = 0;
    max_chatMessage_count=100;//最多在内存中保存100条聊天记录
    
    queue_chatRecords = new ChatMessageInfoList(this);
    
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar40Changed);
    connect (this, &QQItemInfo::settingsChanged, this, &QQItemInfo::avatar240Changed);
    connect (this, &QQItemInfo::nickChanged, this, &QQItemInfo::updataAliasOrNick);
    connect (this, &QQItemInfo::aliasChanged, this, &QQItemInfo::updataAliasOrNick);
    connect (this, &QQItemInfo::isActiveChatPageChanged, this, &QQItemInfo::clearUnreadMessages);
    
    typeString = typeToString (type);
}

QQItemInfo::QQItemInfo(QObject *)
{
    return;
}

QQItemInfo::~QQItemInfo()
{
    queue_chatRecords->clear ();//清除内存中的聊天记录
}

void QQItemInfo::initSettings()
{
    QString userqq = userQQ ();
    QString account = this->account ();
    
    if(account==""||userqq=="")
        return;
    QString name = QDir::homePath ()+"/.webqq/"+userqq+"/"+typeString+"_"+account+"/.config.ini";
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
    return (mytype()!=Discu&&userQQ()!=""&&account()!=""&&mysettings);
}

void QQItemInfo::removeOldChatRecord()
{
    queue_chatRecords->dequeue ()->deleteLater ();//销毁最老的那条消息
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
    if(isCanUseSetting()){
        QString temp_str =  mysettings->value ("avatar-40", "qrc:/images/avatar.png").toString ();
        if(temp_str.left(3)!="qrc")
            temp_str = "file:///"+temp_str;

        return temp_str;
    }
    return "qrc:/images/avatar.png";
}

QString QQItemInfo::avatar240() const
{
    if(isCanUseSetting()){
        QString temp_str =  mysettings->value ("avatar-240", "qrc:/images/avatar.png").toString ();
        if(temp_str.left(3)!="qrc")
            temp_str = "file:///"+temp_str;

        return temp_str;
    }
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

const QString QQItemInfo::typeToString(QQItemInfo::QQItemType type)
{
    switch (type) {
    case Friend:
        return "Friend";
        break;
    case Group:
        return "Group";
    case Discu:
        return "Discu";
    default:
        return "";
    }
}

const QString QQItemInfo::localCachePath(QQItemInfo::QQItemType type, const QString &userqq, const QString &account)
{
    QString typeString = typeToString (type);
    return QDir::homePath ()+"/.webqq/"+userqq+"/"+typeString+"_"+account;
}

QQItemInfo::QQItemType QQItemInfo::mytype() const
{
    return m_mytype;
}

int QQItemInfo::unreadMessagesCount() const
{
    return m_unreadMessagesCount;
}

bool QQItemInfo::isActiveChatPage() const
{
    return m_isActiveChatPage;
}

QString QQItemInfo::account() const
{
    return m_account;
}

void QQItemInfo::setUin(QString arg)
{
    if (m_uin != arg) {
        m_uin = arg;
        if(mytype ()==Discu)//如果是讨论组
            setAccount (arg);//讨论组无真实qq，所以用uin充当
        emit uinChanged ();
        setIsActiveChatPage (false);//默认为false
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

const QString QQItemInfo::localCachePath() const
{
    return QDir::homePath ()+"/.webqq/"+userQQ()+"/"+typeString+"_"+account();
}

ChatMessageInfoList *QQItemInfo::getChatRecords()
{
    clearUnreadMessages();//将未读消息清空
    return queue_chatRecords;//返回所有消息
}

void QQItemInfo::addChatRecord(ChatMessageInfo *data)
{
    if(data!=NULL){
        queue_chatRecords->append (data);//将此条记录加到队列当中
        //qDebug()<<"现在缓冲区中消息的条数为："<<queue_chatRecords->size ();
        if(queue_chatRecords->size ()>max_chatMessage_count){
            removeOldChatRecord ();//移除最老的那条消息
        }
        QQCommand* command = QQCommand::getFirstQQCommand ();
        if(data->senderUin ()!=command->uin ()&&!isActiveChatPage ()){
            //如果发送人不是自己（意思是这条消息不是你发给别人的，而是收到的），并且聊天页面不是活跃的，
            setUnreadMessagesCount (unreadMessagesCount ()+1);//增加未读消息的个数
        }
        command->addRecentContacts (this);//将自己添加到最近联系人列表
    }
}

void QQItemInfo::clearUnreadMessages()
{
    setUnreadMessagesCount (0);
}

void QQItemInfo::setIsActiveChatPage(bool arg)
{
    if (m_isActiveChatPage != arg) {
        m_isActiveChatPage = arg;
        emit isActiveChatPageChanged(arg);
    }
}

ChatMessageInfo *QQItemInfo::getChatMessageInfoById(int messageID)
{
    ChatMessageInfo* info = queue_chatRecords->find (messageID);//先查找这条消息是否存在
    if(info==NULL){//为空证明没有找到
        info = new ChatMessageInfo(messageID, this);
    }
    return info;
}

int QQItemInfo::getMessageIndex()
{
    return messageID++;
}

void QQItemInfo::setUnreadMessagesCount(int arg)
{
    if (m_unreadMessagesCount != arg) {
        m_unreadMessagesCount = arg;
        emit unreadMessagesCountChanged(arg);
    }
}

FriendInfo::FriendInfo(QObject *parent):
    QQItemInfo(Friend, parent)
{
    m_signature = "";
    m_state = Offline;
    m_stateToString = "offline";
    max_chatMessage_count=150;//最大缓存消息数量
    saveRecord_coount=50;//一次将50条消息记录插入到本地（不能大于max_chatMessage_count）
    
    connect (this, &QQItemInfo::settingsChanged, this, &FriendInfo::onSettingsChanged);
    //链接信号，处理settings对象改变的信号
    getChatRecordsing=false;//记录现在是否在请求获取聊天记录
    itemInfoPrivate = DatabaseOperation::createDatabaseOperation ();
}

FriendInfo::~FriendInfo()
{
    saveChatMessageToLocal ();//销毁此对象之前不要忘记将聊天记录存起来
}

QString FriendInfo::QQSignature()
{
    return m_signature;
}

FriendInfo::States FriendInfo::state() const
{
    return m_state;
}

QString FriendInfo::stateToString() const
{
    return m_stateToString;
}

void FriendInfo::removeOldChatRecord()
{
    ChatMessageInfoList list;
    for(int i=0;i<saveRecord_coount;++i){
        list.append (queue_chatRecords->dequeue ());
    }
    QString tableName = "table_"+typeToString ()+account();
    itemInfoPrivate->insertDatas (tableName, &list);
    list.clear ();//保存后记得清空
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
    if(!itemInfoPrivate->openSqlDatabase (userqq)){
        qDebug()<<"FriendInfo:数据库打开失败！";
    }
}

void FriendInfo::closeSqlDatabase()
{
    itemInfoPrivate->closeSqlDatabase ();
}

void FriendInfo::saveChatMessageToLocal()
{
    if(account ()!=""){//qq账户（qq号码）一定不能为空，因为它是消息发送者的唯一标识
        QString tableName = "table_"+typeToString ()+account();
        itemInfoPrivate->insertDatas (tableName, queue_chatRecords);//将所有聊天记录保存下来
        //将内存中的消息添加到数据库
    }
}

void FriendInfo::setState(FriendInfo::States arg)
{
    if (m_state != arg) {
        m_state = arg;
        switch(arg)
        {
        case Online:
            m_stateToString = "online";
            break;
        case Callme:
            m_stateToString = "callme";
            break;
        case Away:
            m_stateToString = "away";
            break;
        case Busy:
            m_stateToString = "busy";
            break;
        case Silent:
            m_stateToString = "silent";
            break;
        case Hidden:
            m_stateToString = "hidden";
            break;
        case Offline:
            m_stateToString = "offline";
            break;
        default:break;
        }
        emit stateChanged(arg);
        emit stateToStringChanged (m_stateToString);
    }
}

void FriendInfo::setStateToString(const QString &str)
{
    if(str!=m_stateToString){
        if(str=="online"){
            setState (Online);
        }else if(str=="callme"){
            setState (Callme);
        }else if(str=="away"){
            setState (Away);
        }else if(str=="busy"){
            setState (Busy);
        }else if(str=="silent"){
            setState (Silent);
        }else if(str=="hidden"){
            setState (Hidden);
        }else if(str=="offline"){
            setState (Offline);
        }
    }
}

void FriendInfo::saveChatMessageToLocal(ChatMessageInfo* data)
{
    if(account ()!=""){//qq账户（qq号码）一定不能为空，因为它是消息发送者的唯一标识
        QString tableName = "table_"+typeToString ()+account();
        itemInfoPrivate->insertData (tableName, data);
    }
}

void FriendInfo::getLocalChatRecords(ChatMessageInfo *currentData, int count)
{
    if(account ()!=""&&!getChatRecordsing){//qq账户（qq号码）一定不能为空，因为它是消息发送者的唯一标识
        QString tableName = "table_"+typeToString ()+account();
        getChatRecordsing = true;//将此值置为true
        ChatMessageInfoList list;
        itemInfoPrivate->getDatas (tableName, count, currentData, &list);
        //开始获取聊天记录
        for(int i=0;i<list.size ();++i){
            queue_chatRecords->insert (i, list.at (i));
        }
    }
}

GroupInfo::GroupInfo(QObject *parent):
    QQItemInfo(Group, parent)
{
    m_code = "";
}

QString GroupInfo::code() const
{
    return m_code;
}

int GroupInfo::membersCount() const
{
    return queue_members.count ();
}

QString GroupInfo::announcement() const
{
    return m_announcement;
}

void GroupInfo::setCode(QString arg)
{
    if (m_code == arg)
        return;
    
    m_code = arg;
    emit codeChanged(arg);
}

void GroupInfo::addMember(FriendInfo *info)
{
    if(!queue_members.contains (info)){//如果不存在
        queue_members<<info;
        emit memberCountChanged (queue_members.count ());
        emit memberIncrease (info);
    }else{
        qDebug()<<"GroupInfo:要增加的群成员已经存在"<<info;
    }
}

void GroupInfo::removeMemberByUin(const QString &uin)
{
    for (int i=0;i<queue_members.count ();++i) {
        FriendInfo* info = queue_members[i];
        if(info&&info->uin ()==uin){
            queue_members.removeAt (i);
            emit memberCountChanged (queue_members.count ());
            emit memberReduce (i);
            break;
        }
    }
}

void GroupInfo::removeMemberByInfo(const FriendInfo *info)
{
    for (int i=0;i<queue_members.count ();++i) {
        FriendInfo* temp_info = queue_members[i];
        if(temp_info==info){
            queue_members.removeAt (i);
            emit memberCountChanged (queue_members.count ());
            emit memberReduce (i);
            break;
        }
    }
}

void GroupInfo::setMemberCard(const QString &uin, const QString &card)
{
    map_card[uin]=card;
}

QString GroupInfo::getMemberCardByUin(const QString &uin, const QString &defaultCard)
{
    return map_card.value (uin, defaultCard);
}

FriendInfo *GroupInfo::getMemberInfoByIndex(int index)
{
    return queue_members[index];
}

void GroupInfo::setAnnouncement(QString arg)
{
    if (m_announcement == arg)
        return;
    
    m_announcement = arg;
    emit announcementChanged(arg);
}

DiscuInfo::DiscuInfo(QObject *parent):
    QQItemInfo(Discu, parent)
{
}

void DiscuInfo::addMember(FriendInfo *info)
{
    if(!queue_members.contains (info)){//如果不存在
        queue_members<<info;
        emit memberCountChanged (queue_members.count ());
        emit memberIncrease (info);
    }
}

void DiscuInfo::removeMemberByUin(const QString &uin)
{
    for (int i=0;i<queue_members.count ();++i) {
        FriendInfo* info = queue_members[i];
        if(info&&info->uin ()==uin){
            queue_members.removeOne (info);
            emit memberCountChanged (queue_members.count ());
            emit memberReduce (i);
            break;
        }
    }
}

void DiscuInfo::removeMemberByInfo(const FriendInfo *info)
{
    for (int i=0;i<queue_members.count ();++i) {
        FriendInfo* temp_info = queue_members[i];
        if(temp_info==info){
            queue_members.removeAt (i);
            emit memberCountChanged (queue_members.count ());
            break;
        }
    }
}

int DiscuInfo::membersCount() const
{
    return queue_members.count ();
}

FriendInfo *DiscuInfo::getMemberInfoByIndex(int index)
{
    return queue_members[index];
}

RecentInfo::RecentInfo(FriendInfo *info, QObject *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToFriend (info);
}

RecentInfo::RecentInfo(GroupInfo *info, QObject *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToGroup (info);
}

RecentInfo::RecentInfo(DiscuInfo *info, QObject *parent):
    QObject(parent)
{
    setInfoData (info);
    setInfoToDiscu (info);
}

QObject *RecentInfo::infoData() const
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

void RecentInfo::setInfoData(QObject *info)
{
    m_infoData = info;
    emit infoDataChanged ();
}

ChatMessageInfoList::ChatMessageInfoList(QObject *parent):
    QObject(parent)
{
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
    list<<obj;
}

void ChatMessageInfoList::insert(int pos, ChatMessageInfo *obj)
{
    list.insert (pos, obj);
}

void ChatMessageInfoList::destroy()
{
    clear();//先清除
    this->deleteLater ();
}

void ChatMessageInfoList::clear()
{
    foreach (ChatMessageInfo* info, list) {
        if(info!=NULL){
            info->deleteLater ();//销毁这条消息
        }
    }
    list.clear ();//清空list;
}

ChatMessageInfo *ChatMessageInfoList::dequeue()
{
    return list.dequeue ();
}

ChatMessageInfo* ChatMessageInfoList::find(int messageID)
{
    foreach (ChatMessageInfo* info, list) {
        if(info!=NULL&&info->messageId ()==messageID)
            return info;
    }
    return NULL;
}
