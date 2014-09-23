#ifndef QQCommand_H
#define QQCommand_H

#include <QObject>
#include <QDebug>
#include <QByteArray>
#include <QTimer>
#include "mymessagebox.h"
#include "mynetworkaccessmanagerfactory.h"

class QQCommand : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString userQQ READ userQQ WRITE setUserQQ NOTIFY userQQChanged)
    Q_PROPERTY(QString userPassword READ userPassword WRITE setUserPassword NOTIFY userPasswordChanged)
    Q_PROPERTY(QQStatus userStatus READ userStatus WRITE setUserStatus NOTIFY userStatusChanged)
    Q_PROPERTY(QString userStatusToString READ userStatusToString NOTIFY userStatusToStringChanged)
    Q_PROPERTY(LoginStatus loginStatus READ loginStatus WRITE setLoginStatus NOTIFY loginStatusChanged)
    Q_PROPERTY(double windowScale READ windowScale WRITE setWindowScale NOTIFY windowScaleChanged)
    Q_ENUMS(QQStatus)
    Q_ENUMS(LoginStatus)
    Q_ENUMS(ProxyType)
   
public:
    explicit QQCommand(QQuickItem *parent = 0);
    enum LoginStatus{
        Offline,//离线
        Logining,//登录中
        LoginFinished//登录完成
    };

    enum QQStatus{
        Offlineing,//离线
        Online,//在线
        Callme,//Q我吧
        Away,//离开
        Busy,//忙碌
        Silent,//请勿打扰
        Hidden//隐身
    };
   
    QString userStatusToString() const
    {
        return m_userStatusToString;
    }
    LoginStatus loginStatus() const
    {
        return m_loginStatus;
    }
    
    QString userQQ() const
    {
        return m_userQQ;
    }
    
    QString userPassword() const
    {
        return m_userPassword;
    }
    
    double windowScale() const
    {
        return m_windowScale;
    }
    
private slots:
    void beginPoll2();
    void poll2Finished(QNetworkReply *replys);
private:
    QQStatus qq_status;
    QQStatus userStatus();
    void setUserStatus( QQStatus new_status );

    QString m_userStatusToString;
    LoginStatus m_loginStatus;

    QByteArray poll2_data;//post心跳包的数据
    QTimer poll2_timer;
    NetworkAccessManager manager;
    QNetworkRequest request;
    QString m_userQQ;
    QString m_userPassword;

    QJSEngine jsEngine;
    void loadApi();
    
    QMap<QString, QString> analysisBasicData( QJsonObject obj );
    void analysisMessage( QJsonObject obj );
    void analysisInputNotify( QJsonObject obj );
    void analysisFriendStatusChanged( QJsonObject obj );
    double m_windowScale;
    
signals:
    void userStatusChanged();
    void userStatusToStringChanged();
    void loginStatusChanged(LoginStatus arg);
    void poll2ReData( QString data );
    void userQQChanged(QString arg);
    void error( QString message );//有错误产生就发送信号
    void updateCode();//刷新验证码
    void inputCodeClose();//关闭验证码输入
    void userPasswordChanged(QString arg);
    
    void windowScaleChanged(double arg);
    
public slots:
    void setLoginStatus(LoginStatus arg);
    void startPoll2( QByteArray data );
    void setUserQQ(QString arg)
    {
        if (m_userQQ != arg) {
            m_userQQ = arg;
            
            emit userQQChanged(arg);
        }
    }
    void setUserPassword(QString arg)
    {
        if (m_userPassword != arg) {
            m_userPassword = arg;
            emit userPasswordChanged(arg);
        }
    }
    void showWarningInfo(QString message);
    void downloadImage( QUrl url, QString uin, QString imageSize, QJSValue callbackFun );
    
    void setValue(const QString & key, const QVariant & value, const QString & userQQ="");
    QVariant getValue(const QString & key, const QVariant & defaultValue = QVariant(), const QString & userQQ="") const;
    void removeValue( const QString & key, const QString & userQQ="" );
    
    void updataApi(const QString content);
    QString getHash();
    QString encryptionPassword(const QString &uin, const QString &code);
    void setWindowScale(double arg)
    {
        if (m_windowScale != arg) {
            m_windowScale = arg;
            emit windowScaleChanged(arg);
        }
    }
};

#endif // QQCommand_H
