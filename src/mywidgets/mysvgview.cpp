#include "mysvgview.h"
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>

#if(QT_VERSION>=0x050000)
MySvgView::MySvgView(QQuickItem *parent) :
    QQuickPaintedItem(parent)
#else
MySvgView::MySvgView(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{
#if(QT_VERSION<0x050000)
    setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
    m_defaultSize = QSize(0,0);
    svg = new QSvgRenderer(this);
}

QUrl MySvgView::source() const
{
    return m_source;
}

QSize MySvgView::defaultSize() const
{
    return m_defaultSize;
}

void MySvgView::setDefaultSize(QSize arg)
{
    if(arg!=m_defaultSize){
        m_defaultSize = arg;
        emit defaultSizeChanged (arg);
    }
}

#if(QT_VERSION>=0x050000)
void MySvgView::paint(QPainter *painter)
#else
void MySvgView::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
#endif
{
    svg->render (painter, boundingRect());
}

void MySvgView::setSource(QUrl arg)
{
    if (m_source != arg) {
        m_source = arg;

        QString str = m_source.toLocalFile();
        if(str == ""){
            str = m_source.toString();
            if(str==""){
                svg->deleteLater();
                svg = new QSvgRenderer(this);
                return;
            }
        }
            
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        svg->load (str);
        setDefaultSize (svg->defaultSize ());
        int width = svg->defaultSize ().width ();
        int height = svg->defaultSize ().height ();
        setImplicitWidth(width);
        setImplicitHeight(height);
        update ();
        emit sourceChanged(arg);
    }
}
