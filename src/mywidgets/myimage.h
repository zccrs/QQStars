#ifndef MYIMAGE_H
#define MYIMAGE_H

#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QtNetwork>
#include <QCache>
#if(QT_VERSION>=0x050000)
#include <QQuickPaintedItem>
#else
#include <QDeclarativeItem>
#endif

#if(QT_VERSION>=0x050000)
class MyImage : public QQuickPaintedItem
#else
class MyImage : public QDeclarativeItem
#endif
{
    Q_OBJECT
    Q_PROPERTY(QUrl maskSource READ maskSource WRITE setMaskSource NOTIFY maskSourceChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(bool grayscale READ grayscale WRITE setGrayscale NOTIFY grayscaleChanged)
    Q_PROPERTY(State status READ status NOTIFY statusChanged FINAL)
    Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(QSize defaultSize READ defaultSize NOTIFY defaultSizeChanged FINAL)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)

    Q_ENUMS(State)
public:
    enum State{
        Null,
        Ready,
        Loading,
        Error
    };

#if(QT_VERSION>=0x050000)
    explicit MyImage(QQuickItem *parent = 0);
#else
    explicit MyImage(QDeclarativeItem *parent = 0);
#endif
    ~MyImage();

    QUrl source() const;
    QUrl maskSource() const;
    bool cache() const;
    bool grayscale() const;
    
    void chromaticToGrayscale(QImage &image);
    static QString imageFormatToString(const QByteArray& array);

#if(QT_VERSION>=0x050000)
    void paint(QPainter * painter);
#else
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *new_style, QWidget *new_widget=0);
#endif
    State status() const;
    QSize sourceSize() const;
    QSize defaultSize() const;

    const QPixmap* getPixmap() const;
signals:
    void sourceChanged(QUrl arg);
    void maskSourceChanged(QUrl arg);
    void loadError();//加载图片出错
    void loadReady();
    void cacheChanged(bool arg);
    void grayscaleChanged(bool arg);
    void statusChanged(State arg);
    void sourceSizeChanged(QSize arg);
    void defaultSizeChanged(QSize arg);

public slots:
    void setSource(QUrl arg);
    void setMaskSource(QUrl arg);
    void setCache(bool arg);
    void setGrayscale(bool arg);
    void setStatus(State arg);
    void reLoad();
    void setSourceSize(QSize arg);
    void setDefaultSize(QSize arg);
    bool save(const QString& fileName) const;

private slots:
    void onDownImageFinished(QNetworkReply* reply);

private:
    QUrl m_source;
    QPixmap *pixmap;
    static QCache<QString, QPixmap> pixmapCache;

    QUrl m_maskSource;
    bool m_cache;
    bool m_grayscale;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    State m_status;
    QSize m_sourceSize;
    QSize m_defaultSize;

    void downloadImage(const QUrl& url);
    void setImage(QImage& image);
};

#endif // MYIMAGE_H
