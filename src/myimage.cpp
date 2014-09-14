#include "myimage.h"
#include <QPainter>
#include <QBitmap>
#include <QDebug>

MyImage::MyImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    connect (this, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect (this, SIGNAL(heightChanged()), SLOT(onHeightChanged()));
}

void MyImage::onWidthChanged()
{
    setImplicitHeight (pixmap.size ().height ()*(width ()/pixmap.size ().width ()));
}
 
void MyImage::onHeightChanged()
{
    setImplicitWidth (pixmap.size ().width ()*(height ()/pixmap.size ().height ()));
}
void MyImage::paint(QPainter *painter)
{
    painter->drawPixmap (0,0,pixmap.scaled (width(), height (), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void MyImage::setSource(QUrl arg)
{
    if (m_source != arg) {
        m_source = arg;
        QString str = arg.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        if( pixmap.load (str) ){
            pixmap.setMask (bitmap.scaled (pixmap.size ()));
            setImplicitSize (pixmap.size ().width (), pixmap.size ().height ());//设置默认大小
            if( width()>0 )
                onWidthChanged();
            if(height ()>0)
                onHeightChanged();
            update ();
        }else{
            emit loadError ();
        }
        emit sourceChanged(arg);
    }
}

void MyImage::setMaskSource(QUrl arg)
{
    if (m_maskSource != arg) {
        m_maskSource = arg;
        QString str = arg.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        bitmap.load (str);
        emit maskSourceChanged(arg);
    }
}
