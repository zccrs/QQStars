#ifndef MYNETWORKACCESSMANAGERFACTORY_H
#define MYNETWORKACCESSMANAGERFACTORY_H

#include <QtQuick>
#include <QtNetwork>
#include <QSslConfiguration>

class MyNetworkAccessManagerFactory : public QObject,public QQmlNetworkAccessManagerFactory
{
    Q_OBJECT
public:
    explicit MyNetworkAccessManagerFactory( QObject *parent = 0 );
    virtual QNetworkAccessManager* create(QObject *parent);
public slots:
    void onIgnoreSSLErrors(QNetworkReply* reply,QList<QSslError> error);
private:
    QMutex mutex;
    QNetworkAccessManager* m_networkManager;
};

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit NetworkAccessManager(QObject *parent = 0);
protected:
    QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
};

class NetworkCookieJar : public QNetworkCookieJar
{
public:
    static NetworkCookieJar* GetInstance();
    void clearCookies();

    virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const;
    virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url);
    QList<QNetworkCookie> cookies() const;
private:
    explicit NetworkCookieJar(QObject *parent = 0);
    ~NetworkCookieJar();
    void load();
    mutable QMutex mutex;
    //QNetworkCookie keepAliveCookie;
};

#endif // MYNETWORKACCESSMANAGERFACTORY_H
