#ifndef QQCommand_H
#define QQCommand_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QTimer>
#include "mymessagebox.h"
#include "mynetworkaccessmanagerfactory.h"
#include "mywindow.h"

class QQItemInfoPrivate:public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(QQItemType)
public:
    enum QQItemType{
        Friend,//好友
        Group,//群
        Discu,//讨论组
    };
};
class QQItemInfo:public QQuickItem
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
    Q_PROPERTY(QQItemInfoPrivate::QQItemType mytype READ mytype NOTIFY mytypeChanged FINAL)

    friend class FriendInfo;
    friend class GroupInfo;
    friend class DiscuInfo;
    friend class RecentInfo;
private:
    explicit QQItemInfo(QQItemInfoPrivate::QQItemType type, QQuickItem *parent=0);
    void initSettings();
    
protected:
    QString m_uin;
    QPointer<QSettings> mysettings;
    QString m_aliasOrNick;
    QString m_userQQ;
    QString m_alias;
    QString typeString;
    bool isCanUseSetting() const;
    QQItemInfoPrivate::QQItemType m_mytype;
public:
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
    QQItemInfoPrivate::QQItemType mytype() const;
    static const QString typeToString(QQItemInfoPrivate::QQItemType type);
    
private slots:
    void updataAliasOrNick();
public slots:
    void setUin(QString arg);
    void setNick(QString arg);
    void setAlias(QString arg);
    void setAccount(QString arg);
    void setAvatar40(QString arg);
    void setAvatar240(QString arg);
    void setUserQQ(QString arg);
    void clearSettings();
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
    void mytypeChanged(QQItemInfoPrivate::QQItemType arg);
};

class FriendInfo:public  QQItemInfo
{
    Q_OBJECT
    Q_PROPERTY(QString QQSignature READ QQSignature WRITE setQQSignature NOTIFY qQSignatureChanged)//个性签名
public:
    explicit FriendInfo(QQuickItem *parent=0);
    QString QQSignature();
public slots:
    void setQQSignature(QString arg);
signals:
    void qQSignatureChanged();
};

class GroupInfo:public  QQItemInfo
{
    Q_OBJECT
public:
    explicit GroupInfo(QQuickItem *parent=0);
};

class DiscuInfo:public  QQItemInfo
{
    Q_OBJECT
public:
    explicit DiscuInfo(QQuickItem *parent=0);
};

class RecentInfo:public  QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem* infoData READ infoData NOTIFY infoDataChanged FINAL)
    Q_PROPERTY(FriendInfo* infoToFriend READ infoToFriend NOTIFY infoToFriendChanged FINAL)
    Q_PROPERTY(GroupInfo* infoToGroup READ infoToGroup NOTIFY infoToGroupChanged FINAL)
    Q_PROPERTY(DiscuInfo* infoToDiscu READ infoToDiscu NOTIFY infoToDiscuChanged FINAL)
public:
    //explicit RecentInfo(QQuickItem *parent=0);
    explicit RecentInfo(FriendInfo *info, QQuickItem *parent=0);
    explicit RecentInfo(GroupInfo *info, QQuickItem *parent=0);
    explicit RecentInfo(DiscuInfo *info, QQuickItem *parent=0);
    QQuickItem* infoData() const;
    FriendInfo* infoToFriend() const;
    GroupInfo* infoToGroup() const;
    DiscuInfo* infoToDiscu() const;
signals:
    void infoDataChanged();
    void infoToFriendChanged(FriendInfo* arg);
    void infoToGroupChanged(GroupInfo* arg);
    void infoToDiscuChanged(DiscuInfo* arg);
private:
    void setInfoData(QQuickItem* info);
    void setInfoToFriend(FriendInfo* arg);
    void setInfoToGroup(GroupInfo* arg);
    void setInfoToDiscu(DiscuInfo* arg);
    QPointer<QQuickItem> m_infoData;
    QPointer<FriendInfo> m_infoToFriend;
    QPointer<GroupInfo> m_infoToGroup;
    QPointer<DiscuInfo> m_infoToDiscu;
};

class QQCommand : public FriendInfo
{
    Q_OBJECT
    Q_PROPERTY(QString userQQ READ userQQ WRITE setUserQQ NOTIFY userQQChanged)
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QQStatus userStatus READ userStatus WRITE setUserStatus NOTIFY userStatusChanged)
    Q_PROPERTY(QString userStatusToString READ userStatusToString NOTIFY userStatusToStringChanged FINAL)
    Q_PROPERTY(LoginStatus loginStatus READ loginStatus WRITE setLoginStatus NOTIFY loginStatusChanged)
    Q_PROPERTY(double windowScale READ windowScale WRITE setWindowScale NOTIFY windowScaleChanged)
    Q_PROPERTY(bool rememberPassword READ rememberPassword WRITE setRememberPassword NOTIFY rememberPasswordChanged)//是否记住密码
    Q_PROPERTY(bool autoLogin READ autoLogin WRITE setAutoLogin NOTIFY autoLoginChanged)//是否自动登录
    Q_PROPERTY(QString codeText READ codeText CONSTANT)
    
    Q_ENUMS(QQStatus)
    Q_ENUMS(LoginStatus)
    Q_ENUMS(SenderType)
    Q_ENUMS(MessageType)
public:
    explicit QQCommand(QQuickItem *parent = 0);
    enum LoginStatus{//登录状态
        Offline,//离线
        Logining,//登录中
        LoginFinished//登录完成
    };

    enum QQStatus{//登录后的用户的qq状态
        Offlineing,//离线中
        Online,//在线
        Callme,//Q我吧
        Away,//离开
        Busy,//忙碌
        Silent,//请勿打扰
        Hidden//隐身
    };
    enum SenderType{//发送消息的人的类型
        Friend,//好友
        Group,//群
        Discu,//讨论组
        Stranger,//陌生人
        SystemMessage//系统消息，包含群管理员的更改，成员的更改，还有好友验证消息
    };
    
    enum MessageType{//消息的类型
        InputNotify,//正在输入
        Text,//文本
        Image,//图片
        Face,//表情
        GeneralMessage,//普通消息，包含Text Image Face等
        //SendFile,//发送文件
        //CancleSendFile,//取消发送文件
        FileMessage,//文件消息，包含发送文件和取消发送文件
        AvRequest,//请求开视频
        AvRefuse,//取消开视频
        ShakeWindow,//窗口抖动
        FriendStatusChanged,//好友状态改变
        FriendVerify,//好友验证消息poll_type=system_message，type=verify_required
        GroupAdmin,//群管理员消息poll_type=sys_g_msg，type=group_admin_op，uin_flag=1为设为管理员，0为取消管理员
        GroupLeave//群T人的消息
    };
    
    QString userStatusToString() const;
    LoginStatus loginStatus() const;
    QString userQQ() const;
    QString userPassword() const;
    double windowScale() const;
    bool rememberPassword() const;
    bool autoLogin() const;
    QString codeText() const;
    
private slots:
    void setStatusToString();//根据当前qq状态设置statusToString
    void beginPoll2();//启动心跳包
    void poll2Finished(QNetworkReply *replys);//qq心跳包获取完成时调用
    void initUserPassword();//初始化用户密码(从QSettings中)
    void onChatMainWindowClose();//接收主聊天窗口关闭的信号
private:
    QQStatus qq_status;
    QQStatus userStatus();
    void setUserStatus( QQStatus new_status );

    QString m_userStatusToString;//储存登录状态的string类型的数据
    LoginStatus m_loginStatus;//储存当前用户的登录状态
    QByteArray poll2_data;//post心跳包的数据
    NetworkAccessManager manager;//储存管理心跳包网络请求的对象
    QNetworkRequest request;//储存发送心跳包的网络请求对象
    QString m_userQQ;//储存当前用户qq号码
    QString m_userPassword;//储存当前用户密码
    QPointer<MyWindow> code_window;//储存指向输入验证码窗口的指针
    QJSEngine jsEngine;//储存加载了api(*.js文件)的js引擎
    double m_windowScale;//储存可视控件的比例
    QString m_codeText;//储存输入验证码
    QPointer<MyWindow> warning_info_window;//储存指向警告窗口的指针
    QMap<QString, QQItemInfo*> map_itemInfo;//储存每个好友或群讨论组Info
    QMap<QString, QString> map_alias;//储存备注名
    QPointer<QQuickWindow> mainChatWindowCommand;//储存所有聊天窗口的主管理窗口
    QQuickItem* mainChatWindowCommand_item;//储存每一个聊天页面的父对象(聊天窗口anchors.fill此父对象)
    QMap<QString, QQuickItem*> map_chatPage;//储存备已经打开的聊天页面
    
    struct FontStyle{
        int size;//字体大小
        QString color;//字体颜色
        bool bold;//加黑
        bool italic;//斜体
        bool underline;//下划线
        QString family;//字体
    };

    void loadApi();
    QString disposeMessage( QJsonObject &obj );//解析基本消息
    //void disposeInputNotify( QJsonObject &obj );//处理好友正在输入消息
    void disposeFriendStatusChanged( QJsonObject &obj );//处理好友状态改变
    void disposeFriendMessage( QJsonObject &obj, MessageType type=GeneralMessage );//处理好友消息
    void disposeGroupMessage( QJsonObject &obj, MessageType type=GeneralMessage );//处理群消息
    void disposeDiscuMessage( QJsonObject &obj, MessageType type=GeneralMessage );//处理讨论组消息
    void disposeStrangerMessage( QJsonObject &obj, MessageType type=GeneralMessage );//处理陌生人消息
    void disposeSystemMessage(QJsonObject &obj);//处理系统消息
    //void disposeFileMessage( QJsonObject &obj );//处理文件传输方面的消息
    //void disposeAvMessage( QJsonObject &obj, bool open/*true为开视频，false为取消开视频*/ );//处理视频聊天方面的消息
    //void disposeShakeMessage( QJsonObject &obj );
    QString doubleToString( QJsonObject &obj, QString name );//将obj中类型为double的数据转化为QString类型
    QString textToHtml(FontStyle &style, QString data);//将文本内容转化为富文本

signals:
    void userStatusChanged();
    void userStatusToStringChanged();
    void loginStatusChanged();
    void poll2ReData( QString data );
    void userQQChanged();
    void error( QString message );//有错误产生就发送信号
    void userPasswordChanged();
    
    void windowScaleChanged();//窗口比例改变
    void messageArrive(SenderType senderType, QString uin, QString jsonData);
    void rememberPasswordChanged();
    void autoLoginChanged();

    void friendInputNotify(QString fromUin);//好友正在输入的信号
    void newMessage(QString fromUin, QString senderUin, QString message);//新的聊天消息信号
    void shakeWindow(QString fromUin);//窗口抖动信号
    void friendStatusChanged(QString fromUin, QString newStatus);//好友状态改变的信号
    void addChatPage(QQuickItem* item);//增加聊天页面的信号
    void activeChatPageChanged(QQuickItem* item);//将item这个page变为活跃的page
public slots:
    void setRememberPassword(bool arg);
    void setAutoLogin(bool arg);
    void saveUserPassword();
    void setLoginStatus(LoginStatus arg);
    void startPoll2( QByteArray data );
    void setUserQQ(QString arg);
    void setUserPassword(QString arg);
    void setWindowScale(double arg);
    void saveAlias(int type, QString uin, QString alias);//储存备注名称
    
    QString getHash();//获取请求好友列表需要的hsah
    QString encryptionPassword(const QString &uin, const QString &code);//加密密码，用来登录
    
    int openMessageBox( QJSValue value );//打开一个对话窗口
    void showWarningInfo(QString message);//显示一个警告窗口
    void downloadImage( QUrl url, QString uin, QString imageSize, QJSValue callbackFun );//下载图片
    void showCodeWindow(const QJSValue callbackFun, const QString code_uin);//显示出输入验证码的窗口
    void closeCodeWindow();//关闭输入验证码的窗口
    void updataCode();//刷新验证码的显示
    void updataApi(const QString content);//重新载入api.js，用于更新api后的操作
    
    FriendInfo* createFriendInfo(const QString uin);//创建一个储存好友信息的对象
    GroupInfo* createGroupInfo(const QString uin);//创建一个储存群信息的对象
    DiscuInfo* createDiscuInfo(const QString uin);//创建一个储存讨论组信息的对象
    
    void addChatWindow(QString uin, int senderType/*QQItemType类型*/);//新增聊天窗口
    int removeChatPage(QString uin, int senderType/*QQItemType类型*/);//移除已有的聊天Page
    
    QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;//返回储存在QSettings里边的value;
    void setValue(const QString & key, const QVariant & value);
};
#endif // QQCommand_H
