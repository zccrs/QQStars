#ifndef QQITEMINFO_H
#define QQITEMINFO_H

#include <QQuickItem>
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
    ChatMessageInfo(QObject* parent = 0);
    ChatMessageInfo(int messageID, QObject* parent = 0);
    
    QString senderUin() const;
    QString contentData() const;
    QDate date() const;
    QTime time() const;
    int messageId() const;
    int messageId2() const;
    
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
    ~ChatMessageInfoList();
private:
    QQueue<ChatMessageInfo*> list;
public slots:
    ChatMessageInfo* at(int i);
    int length();
    int size();
    void append( ChatMessageInfo *obj);
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
    QThread *thread;
    DatabaseOperation();
    ~DatabaseOperation();
    bool tableAvailable(const QString& tableName);//判断表名为tableName的表是可操作
private slots:
    void m_openSqlDatabase(const QString& userqq);//初始化数据库
    void m_insertDatas(const QString& tableName, ChatMessageInfoList* datas);//向数据库中插入多条数据
    void m_getDatas(const QString& tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList *datas);
    //获取数据库中currentData这条数据之前的count条数据，将获得的数据存入datas当中
signals:
    void sql_open(const QString& userqq);
    void sql_insertDatas(const QString& tableName, ChatMessageInfoList* datas);//向数据库中插入多条数据
    void sql_getDatas(const QString& tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList* datas);
    void getDatasFinished(ChatMessageInfoList* datas);//获取多条数据完成
public slots:
    void openSqlDatabase(const QString& userqq);//初始化数据库
    void closeSqlDatabase();
    void insertData(const QString& tableName, ChatMessageInfo *data);//向数据库中插入数据
    void insertDatas(const QString& tableName, ChatMessageInfoList* datas);//向数据库中插入多条数据
    void getDatas(const QString& tableName, int count, ChatMessageInfo* currentData, ChatMessageInfoList* datas);
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
        Discu,//讨论组
    };
private:
    explicit QQItemInfo(QQItemType type, QObject *parent=0);
    void initSettings();
    void setUnreadMessagesCount(int arg);//设置未读消息的个数
    bool m_isActiveChatPage;
    int messageID;//为自己个好友发送的消息的id（为此消息的唯一标识）
    
protected:
    QString m_uin;//uin，为此qq的唯一标识
    QString m_account;//qq账号
    QPointer<QSettings> mysettings;
    QString m_aliasOrNick;
    QString m_userQQ;
    QString m_nick;//储存昵称
    QString m_alias;
    QString typeString;
    int m_unreadMessagesCount;
    QQItemType m_mytype;
    ChatMessageInfoList *queue_chatRecords;//储存聊天记录的队列
    bool isCanUseSetting() const;//是否可以调用settings
    QTimer m_timer;//此定时器用于当聊天页面被销毁后在内存中保存聊天的时常，此定时器触发后会调用虚槽函数
protected slots:
    virtual void clearChatRecords();//清空聊天记录
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
    void clearSettings();
    const QString localCachePath() const;//本地缓存路径
    virtual QVariant getChatRecords();//将内存中的聊天记录读回，如果无记录就返回空的QVariant类型
    //const ChatMessageInfoList* getChatRecordsList();//将储存聊天记录的队列返回
    void addChatRecord(ChatMessageInfo *data);//增加聊天记录，记录在内存当中
    void startClearChatRecordsTimer();//启动清空聊天记录的定时器
    void stopClearChatRecordsTimer();//停止情况聊天记录的定时器
    void setIsActiveChatPage(bool arg);
    ChatMessageInfo* createChatMessageInfo(int messageID);//创建一个储存聊天记录的信息的对象
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
        Offlineing,//离线中
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
protected slots:
    virtual void onSettingsChanged();//处理settings对象改变的信号
private slots:
    void clearChatRecords();//清空聊天记录
public slots:
    void setQQSignature(QString arg);
    void openSqlDatabase(const QString &userqq);//初始化数据库
    void closeSqlDatabase();
    void getLocalChatRecords(ChatMessageInfo *currentData, int count);//读取本地聊天记录（从数据库）
    void saveChatMessageToLocal(ChatMessageInfo *data);//将此消息记录保存到到本地（保存到数据库中）
    void saveChatMessageToLocal();//将当前内存中的消息记录保存到到本地（保存到数据库中）
    QVariant getChatRecords();//将内存中的聊天记录读回，如果无记录就返回空的QVariant类型
    void setState(States arg);
    void setStateToString(const QString &str);
signals:
    void qQSignatureChanged();
    void httpGetQQSignature();//发送信号告诉qml端去获取个性签名
    void getLocalChatRecordsFinished(ChatMessageInfoList* datas);//如果从数据库获取多条聊天记录完成，qml端会接收此信号
    void stateChanged(States arg);
    void stateToStringChanged(QString arg);
};

class GroupInfo:public  QQItemInfo
{
    Q_OBJECT
    Q_PROPERTY(QString code READ code WRITE setCode NOTIFY codeChanged)
    QString m_code;
    
public:
    explicit GroupInfo(QObject *parent=0);
    QString code() const;
public slots:
    void setCode(QString arg);
signals:
    void codeChanged(QString arg);
};

class DiscuInfo:public  QQItemInfo
{
    Q_OBJECT
public:
    explicit DiscuInfo(QObject *parent=0);
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
