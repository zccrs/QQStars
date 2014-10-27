#ifndef MYIMAGE_H
#define MYIMAGE_H

#include <QQuickPaintedItem>
#include <QImage>
#include <QPixmap>
#include <QBitmap>

class MyImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QUrl maskSource READ maskSource WRITE setMaskSource NOTIFY maskSourceChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(bool grayscale READ grayscale WRITE setGrayscale NOTIFY grayscaleChanged)
public:
    explicit MyImage(QQuickItem *parent = 0);

    QUrl source() const;
    QUrl maskSource() const;
    bool cache() const;
    bool grayscale() const;
    
    QImage chromaticToGrayscale(const QImage &image);
private:
    QUrl m_source;
    QPixmap *pixmap;
    QBitmap *bitmap;
    QUrl m_maskSource;
    bool m_cache;
    bool m_grayscale;

protected:
    void paint(QPainter * painter);
signals:
    void sourceChanged(QUrl arg);
    void maskSourceChanged(QUrl arg);
    void loadError();//加载图片出错
    void cacheChanged(bool arg);
    void grayscaleChanged(bool arg);
private slots:
    void onWidthChanged();
    void onHeightChanged();
public slots:
    void setSource(QUrl arg);
    void setMaskSource(QUrl arg);
    void setCache(bool arg);
    void setGrayscale(bool arg);
};

#endif // MYIMAGE_H
