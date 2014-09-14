#ifndef MYSVGVIEW_H
#define MYSVGVIEW_H

#include <QQuickPaintedItem>
#include <QSvgRenderer>

class MySvgView : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    QSvgRenderer *svg;
    QUrl m_source;
public:
explicit MySvgView(QQuickItem *parent = 0);
QUrl source() const
{
    return m_source;
}

private slots:
    void onWidthChanged();
    void onHeightChanged();
protected:
    void paint(QPainter * painter);
    //QSGNode *updatePaintNode(QSGNode * oldNode, UpdatePaintNodeData * updatePaintNodeData);
signals:
    void sourceChanged(QUrl arg);
    
    void rotationModeChanged(Qt::Axis arg);
    
    void rotationOriginChanged(QPoint arg);

public slots:
void setSource(QUrl arg);
};

#endif // MYSVGVIEW_H
