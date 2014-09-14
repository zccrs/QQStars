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
    QUrl m_source;
    QPixmap pixmap;
    QBitmap bitmap;
    QUrl m_maskSource;
    
public:
    explicit MyImage(QQuickItem *parent = 0);

QUrl source() const
{
    return m_source;
}
QUrl maskSource() const
{
    return m_maskSource;
}

protected:
    void paint(QPainter * painter);
signals:
    void sourceChanged(QUrl arg);
    void maskSourceChanged(QUrl arg);
    void loadError();//加载图片出错
private slots:
    void onWidthChanged();
    void onHeightChanged();
public slots:

    void setSource(QUrl arg);
    void setMaskSource(QUrl arg);
};

#endif // MYIMAGE_H
