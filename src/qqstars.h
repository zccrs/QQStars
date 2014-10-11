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
    Q_PROPERTY(QString aliasOrNick READ aliasOrNick NOTIFY aliasOrNickChanged CONSTANT)
    Q_PROPERTY(QString avatar40 READ avatar40 WRITE setAvatar40 NOTIFY avatar40Changed)
    Q_PROPERTY(QString avatar240 READ avatar240 WRITE setAvatar240 NOTIFY avatar240Changed)
    Q_PROPERTY(QString account READ account WRITE setAccount NOTIFY accountChanged)
    Q_PROPERTY(QQItemInfoPrivate::QQItemType mytype READ mytype NOTIFY mytypeChanged CONSTANT)

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
    Q_PROPERTY(QQuickItem* infoData READ infoData NOTIFY infoDataChanged CONSTANT)
    Q_PROPERTY(FriendInfo* infoToFriend READ infoToFriend NOTIFY infoToFriendChanged CONSTANT)
    Q_PROPERTY(GroupInfo* infoToGroup READ infoToGroup NOTIFY infoToGroupChanged CONSTANT)
    Q_PROPERTY(DiscuInfo* infoToDiscu READ infoToDiscu NOTIFY infoToDiscuChanged CONSTANT)
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
    Q_PROPERTY(QString userStatusToString READ userStatusToString NOTIFY userStatusToStringChanged CONSTANT)
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
    enum LoginStatus{
        Offline,//离线
        Logining,//登录中
        LoginFinished//登录完成
    };

    enum QQStatus{
        Offlineing,//离线中
        Online,//在线
        Callme,//Q我吧
        Away,//离开
        Busy,//忙碌
        Silent,//请勿打扰
        Hidden//隐身
    };
    enum SenderType{
        Friend,//好友
        Group,//群
        Discu,//讨论组
        Stranger,//陌生人
        SystemMessage//系统消息，包含群管理员的更改，成员的更改，还有好友验证消息
    };
    
    enum MessageType{
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
    void setStatusToString();
    void beginPoll2();
    void poll2Finished(QNetworkReply *replys);
    void initUserPassword();
private:
    QQStatus qq_status;
    QQStatus userStatus();
    void setUserStatus( QQStatus new_status );

    QString m_userStatusToString;
    LoginStatus m_loginStatus;

    QByteArray poll2_data;//post心跳包的数据
    
    NetworkAccessManager manager;
    QNetworkRequest request;
    QString m_userQQ;
    QString m_userPassword;

    QPointer<MyWindow> code_window;
    
    QJSEngine jsEngine;
    void loadApi();
    
    QString analysisMessage( QJsonObject &obj );//解析基本消息
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
    double m_windowScale;
    QString m_codeText;
    QPointer<MyWindow> warning_info_window;
    QMap<QString, QQItemInfo*> map_itemInfo;
    QMap<QString, QString> map_alias;
signals:
    void userStatusChanged();
    void userStatusToStringChanged();
    void loginStatusChanged();
    void poll2ReData( QString data );
    void userQQChanged();
    void error( QString message );//有错误产生就发送信号
    void userPasswordChanged();
    
    void windowScaleChanged();
    void messageArrive(SenderType senderType, QString uin, QString jsonData);
    void rememberPasswordChanged();
    void autoLoginChanged();

public slots:
    void setLoginStatus(LoginStatus arg);
    void startPoll2( QByteArray data );
    void setUserQQ(QString arg);
    void setUserPassword(QString arg);
    void showWarningInfo(QString message);
    void downloadImage( QUrl url, QString uin, QString imageSize, QJSValue callbackFun );
    void showCodeWindow(const QJSValue callbackFun, const QString code_uin);
    void closeCodeWindow();
    void updataCode();//刷新验证码的显示
    
    FriendInfo* createFriendInfo(const QString uin);
    GroupInfo* createGroupInfo(const QString uin);
    DiscuInfo* createDiscuInfo(const QString uin);
    RecentInfo* createRecentInfo(QQItemInfoPrivate::QQItemType type, const QString uin);
    void saveAlias(int type, QString uin, QString alias);//储存备注名称
    void updataApi(const QString content);
    QString getHash();
    QString encryptionPassword(const QString &uin, const QString &code);
    void setWindowScale(double arg);
    
    int openMessageBox( QJSValue value );
    void setRememberPassword(bool arg);
    void setAutoLogin(bool arg);
    
    void saveUserPassword();
};
#endif // QQCommand_H
