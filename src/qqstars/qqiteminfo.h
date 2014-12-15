#ifndef QQITEMINFO_H
#define QQITEMINFO_H

#include <QtSql>
#include <QSettings>
#include <QObject>
#include <QDate>
#include <QTime>
#include <QQueue>
#include <QThread>
#include <QPointer>
#include <QTimer>
#include <QString>
#include <QDir>

class QQItemInfo;
class ChatMessageInfo:public QObject//用来储存聊天消息的各种信息
{
    Q_OBJECT
    Q_PROPERTY(QString senderUin READ senderUin WRITE setSenderUin NOTIFY senderUinChanged)//储存发送人的uin
    Q_PROPERTY(QString contentData READ contentData WRITE setContentData NOTIFY contentDataChanged)//储存消息内容
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)//储存收到此条信息的日期
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)//储存收到此条信息的时间
    Q_PROPERTY(int messageId READ messageId)
    Q_PROPERTY(int messageId2 READ messageId2 WRITE setMessageId2 NOTIFY messageId2Changed)
    
public:
    ChatMessageInfo(QQItemInfo* parent=0);
    ChatMessageInfo(int messageID, QQItemInfo* parent);
    
    QString senderUin() const;
    QString contentData() const;
    QDate date() const;
    QTime time() const;
    int messageId() const;
    int messageId2() const;
    const QQItemInfo* getParent();//获取此条消息所属的ItemInfo对象
private:
    QString m_senderUin;
    QString m_contentData;
    QDate m_date;
    QTime m_time;
    int m_messageId;
    int m_messageId2;
    
signals:
    void senderUinChanged(QString arg);
    void contentDataChanged(QString arg);
    void dateChanged(QDate arg);
    void timeChanged(QTime arg);
    void messageId2Changed(int arg);

public slots:
    void setSenderUin(QString arg);
    void setContentData(QString arg);
    void setDate(QDate arg);
    void setTime(QTime arg);
    void setMessageId2(int arg);
};

class ChatMessageInfoList:public QObject
{
    Q_OBJECT
public:
    ChatMessageInfoList(QObject* parent=0);
private:
    QQueue<ChatMessageInfo*> list;
public slots:
    ChatMessageInfo* at(int i);
    int length();
    int size();
    void append( ChatMessageInfo *obj);
    void insert(int pos, ChatMessageInfo *obj);
    void destroy();
    void clear();//清除储存的聊天记录
    ChatMessageInfo* dequeue();//出队
    ChatMessageInfo *find(int messageID);
};

class DatabaseOperation:public QObject//提供数据库的操作（用于储存聊天记录）
{
    Q_OBJECT
public:
    static DatabaseOperation* createDatabaseOperation();
private:
    static QSqlDatabase sqlite_db;
    DatabaseOperation();
    ~DatabaseOperation();
    bool tableAvailable(const QString& tableName);//判断表名为tableName的表是可操作

public slots:
    bool openSqlDatabase(const QString& userqq);//初始化数据库
    void closeSqlDatabase();
    void insertData(const QString& tableName, ChatMessageInfo *data);//向数据库中插入数据
    void insertDatas(const QString& tableName, ChatMessageInfoList *datas);//向数据库中插入多条数据
    void getDatas(const QString& tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList *datas);
    //获取数据库中的count条数据，将获得的数据存入datas当中
};

class QQItemInfo:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userQQ READ userQQ WRITE setUserQQ NOTIFY userQQChanged)
    Q_PROPERTY(QString uin READ uin WRITE setUin NOTIFY uinChanged)
    Q_PROPERTY(QString nick READ nick WRITE setNick NOTIFY nickChanged)
    Q_PROPERTY(QString alias READ alias WRITE setAlias NOTIFY aliasChanged)
    Q_PROPERTY(QString aliasOrNick READ aliasOrNick NOTIFY aliasOrNickChanged FINAL)
    Q_PROPERTY(QString avatar40 READ avatar40 WRITE setAvatar40 NOTIFY avatar40Changed)
    Q_PROPERTY(QString avatar240 READ avatar240 WRITE setAvatar240 NOTIFY avatar240Changed)
    Q_PROPERTY(QString account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(int unreadMessagesCount READ unreadMessagesCount NOTIFY unreadMessagesCountChanged FINAL)//未读消息的条数
    Q_PROPERTY(QQItemType mytype READ mytype NOTIFY mytypeChanged FINAL)
    Q_PROPERTY(bool isActiveChatPage READ isActiveChatPage WRITE setIsActiveChatPage NOTIFY isActiveChatPageChanged)
    
    Q_ENUMS(QQItemType)
    friend class FriendInfo;
    friend class GroupInfo;
    friend class DiscuInfo;
    friend class RecentInfo;
public:
    explicit QQItemInfo(QObject *parent=0);
    ~QQItemInfo();
    enum QQItemType{
        Friend,//好友
        Group,//群
        Discu//讨论组
    };
    
private:
    explicit QQItemInfo(QQItemType type, QObject *parent=0);
    void initSettings();
    void setUnreadMessagesCount(int arg);//设置未读消息的个数
    
    QString m_uin;//uin，为此qq的唯一标识
    QString m_account;//qq账号
    QString m_aliasOrNick;
    QString m_userQQ;
    QString m_nick;//储存昵称
    QString m_alias;
    QString typeString;
    int m_unreadMessagesCount;
    QQItemType m_mytype;
    bool m_isActiveChatPage;
    int messageID;//为自己个好友发送的消息的id（为此消息的唯一标识）
protected:
    ChatMessageInfoList* queue_chatRecords;//储存聊天记录的队列
    QPointer<QSettings> mysettings;
    int max_chatMessage_count;//在queue_chatRecords中最多保存多少条聊天记录
    
    bool isCanUseSetting() const;//是否可以调用settings
    virtual void removeOldChatRecord();//将最老的那条消息清除
public:
    static const QString typeToString(QQItemType type);
    static const QString localCachePath(QQItemType type, const QString& userqq, const QString& account);//本地缓存路径
    QString uin() const;
    QString nick() const;
    QString alias() const;
    QString avatarSource() const;
    QString account() const;
    QString avatar40() const;
    QString avatar240() const;
    QString aliasOrNick();
    QString userQQ() const;
    QString typeToString();
    QQItemType mytype() const;
    int unreadMessagesCount() const;
    bool isActiveChatPage() const;
    
private slots:
    void updataAliasOrNick();
    void clearUnreadMessages();//清空未读消息
public slots:
    void setUin(QString arg);
    void setNick(QString arg);
    void setAlias(QString arg);
    void setAccount(QString arg);
    void setAvatar40(QString arg);
    void setAvatar240(QString arg);
    void setUserQQ(QString arg);
    void clearSettings();//清除配置信息
    const QString localCachePath() const;//本地缓存路径
    ChatMessageInfoList *getChatRecords();//将内存中的聊天记录读回
    void addChatRecord(ChatMessageInfo *data);//增加聊天记录，记录在内存当中
    void setIsActiveChatPage(bool arg);
    ChatMessageInfo* getChatMessageInfoById(int messageID);//通过messageID返回一个储存聊天记录的信息的对象
    int getMessageIndex();//返回一个本次在线中的一个唯一的数字，用于收到或者发送的消息的id
    
signals:
    void nickChanged();
    void aliasChanged();
    void accountChanged();
    void avatar40Changed();
    void avatar240Changed();
    void aliasOrNickChanged();
    void userQQChanged();
    void uinChanged();
    void settingsChanged();
    void mytypeChanged(QQItemType arg);
    void unreadMessagesCountChanged(int arg);
    void isActiveChatPageChanged(bool arg);
};

class FriendInfo:public  QQItemInfo
{
    Q_OBJECT
    Q_PROPERTY(QString QQSignature READ QQSignature WRITE setQQSignature NOTIFY qQSignatureChanged)//个性签名
    Q_PROPERTY(States state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString stateToString READ stateToString WRITE setStateToString NOTIFY stateToStringChanged)
    Q_ENUMS(States)
public:
    explicit FriendInfo(QObject *parent=0);
    ~FriendInfo();
    QString QQSignature();
    
    enum States{//登录后的用户的qq状态
        Offline,//离线中
        Online,//在线
        Callme,//Q我吧
        Away,//离开
        Busy,//忙碌
        Silent,//请勿打扰
        Hidden//隐身
    };
    States state() const;
    QString stateToString() const;
    
private:
    QString m_signature;//用来储存个性签名
    DatabaseOperation *itemInfoPrivate;//里边定义了数据库的操作，用来储存聊天记录
    bool getChatRecordsing;//记录现在是否正在请求获取本地聊天记录
    States m_state;
    QString m_stateToString;
    int saveRecord_coount;//记录一次插入多少条聊天记录到数据库
    
    void removeOldChatRecord();//当消息数量超过最大缓存量时被调用
protected slots:
    virtual void onSettingsChanged();//处理settings对象改变的信号
public slots:
    void setQQSignature(QString arg);
    void openSqlDatabase(const QString &userqq);//初始化数据库
    void closeSqlDatabase();
    void getLocalChatRecords(ChatMessageInfo *currentData, int count);//读取本地聊天记录（从数据库）
    void saveChatMessageToLocal(ChatMessageInfo *data);//将此消息记录保存到到本地（保存到数据库中）
    void saveChatMessageToLocal();//将当前内存中的消息记录保存到到本地（保存到数据库中）
    void setState(States arg);
    void setStateToString(const QString &str);
signals:
    void qQSignatureChanged();
    void httpGetQQSignature();//发送信号告诉qml端去获取个性签名
    void stateChanged(States arg);
    void stateToStringChanged(QString arg);
};

class GroupInfo:public  QQItemInfo
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)//相当于好友的uin的功能
    Q_PROPERTY(int membersCount READ membersCount NOTIFY memberCountChanged FINAL)//群成员个数
    Q_PROPERTY(QString announcement READ announcement WRITE setAnnouncement NOTIFY announcementChanged)
    
    QString m_code;
    QQueue<FriendInfo*> queue_members;//储存群成员列表
    QMap<QString, QString> map_card;
    QString m_announcement;

public:
    explicit GroupInfo(QObject *parent=0);
    QString code() const;
    int membersCount() const;
    QString announcement() const;
    
public slots:
    void setCode(QString arg);
    void addMember(FriendInfo* info);//增加群成员
    void removeMemberByUin(const QString& uin);//删除群成员（根据uin）
    void removeMemberByInfo(const FriendInfo *info);//删除群成员（根据uin）
    void setMemberCard(const QString& uin, const QString& card);//给群成员设置群名片
    QString getMemberCardByUin(const QString& uin, const QString& defaultCard);
    FriendInfo* getMemberInfoByIndex(int index);//获取群成员信息
    void setAnnouncement(QString arg);
signals:
    void codeChanged(QString arg);
    void memberCountChanged(int arg);
    void memberIncrease(FriendInfo* info);//群成员增加了
    void memberReduce(int index);//群成员减少了,index为被移除的群成员序号
    void announcementChanged(QString arg);
};

class DiscuInfo:public  QQItemInfo
{
    Q_OBJECT
    Q_PROPERTY(int membersCount READ membersCount NOTIFY memberCountChanged FINAL)//讨论组成员个数
    
    QQueue<FriendInfo*> queue_members;//储存讨论组成员列表
public:
    explicit DiscuInfo(QObject *parent=0);
    int membersCount() const;
public slots:
    void addMember(FriendInfo* info);//增加群成员
    void removeMemberByUin(const QString& uin);//删除群成员（根据uin）
    void removeMemberByInfo(const FriendInfo *info);//删除群成员（根据uin）
    FriendInfo* getMemberInfoByIndex(int index);//获取讨论组成员信息
signals:
    void memberCountChanged(int arg);
    void memberIncrease(FriendInfo* info);//群成员增加了
    void memberReduce(int index);//群成员减少了,index为被移除的群成员序号
};

class RecentInfo:public  QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* infoData READ infoData NOTIFY infoDataChanged FINAL)
    Q_PROPERTY(FriendInfo* infoToFriend READ infoToFriend NOTIFY infoToFriendChanged FINAL)
    Q_PROPERTY(GroupInfo* infoToGroup READ infoToGroup NOTIFY infoToGroupChanged FINAL)
    Q_PROPERTY(DiscuInfo* infoToDiscu READ infoToDiscu NOTIFY infoToDiscuChanged FINAL)
public:
    //explicit RecentInfo(QQuickItem *parent=0);
    explicit RecentInfo(FriendInfo *info, QObject *parent=0);
    explicit RecentInfo(GroupInfo *info, QObject *parent=0);
    explicit RecentInfo(DiscuInfo *info, QObject *parent=0);
    QObject* infoData() const;
    FriendInfo* infoToFriend() const;
    GroupInfo* infoToGroup() const;
    DiscuInfo* infoToDiscu() const;
signals:
    void infoDataChanged();
    void infoToFriendChanged(FriendInfo* arg);
    void infoToGroupChanged(GroupInfo* arg);
    void infoToDiscuChanged(DiscuInfo* arg);
private:
    void setInfoData(QObject* info);
    void setInfoToFriend(FriendInfo* arg);
    void setInfoToGroup(GroupInfo* arg);
    void setInfoToDiscu(DiscuInfo* arg);
    QPointer<QObject> m_infoData;
    QPointer<FriendInfo> m_infoToFriend;
    QPointer<GroupInfo> m_infoToGroup;
    QPointer<DiscuInfo> m_infoToDiscu;
};

#endif // QQITEMINFO_H
