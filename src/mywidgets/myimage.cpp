#include "myimage.h"
#include <QPainter>
#include <QBitmap>
#include <QDebug>

#if(QT_VERSION>=0x050000)
MyImage::MyImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
#else
MyImage::MyImage(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{
    m_cache = true;
    m_grayscale = false;

    pixmap = new QPixmap;
    bitmap = new QBitmap;
    connect (this, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect (this, SIGNAL(heightChanged()), SLOT(onHeightChanged()));
}

QUrl MyImage::source() const
{
    return m_source;
}

QUrl MyImage::maskSource() const
{
    return m_maskSource;
}

bool MyImage::cache() const
{
    return m_cache;
}

bool MyImage::grayscale() const
{
    return m_grayscale;
}

QImage MyImage::chromaticToGrayscale(const QImage &image)
{
    if(image.isGrayscale ())
        return image;
    QImage iGray(image.size (), QImage::Format_ARGB32_Premultiplied);
    for(int i=0;i<image.width ();++i){
        for(int j=0;j<image.height ();++j){
            QRgb pixel = image.pixel(i,j);
            pixel = qGray (pixel);
            pixel = qRgb(pixel,pixel,pixel);            
            iGray.setPixel (i,j,pixel);
        }
    }
    return iGray;
}

void MyImage::onWidthChanged()
{
    setImplicitHeight (pixmap->size ().height ()*(width ()/pixmap->size ().width ()));
}
 
void MyImage::onHeightChanged()
{
    setImplicitWidth (pixmap->size ().width ()*(height ()/pixmap->size ().height ()));
}
void MyImage::paint(QPainter *painter)
{
    painter->drawPixmap (0,0,pixmap->scaled (width(), height (), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void MyImage::setSource(QUrl arg)
{
    if (!m_cache||m_source != arg) {
        m_source = arg;
        QString str;
        str = arg.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        QImage image;
        if( image.load (str)){
            if(m_grayscale){//如果为黑白
                image = chromaticToGrayscale(image);//转换为黑白图
            }
            *pixmap = QPixmap::fromImage (image);
            pixmap->setMask (bitmap->scaled (pixmap->size ()));
            setImplicitWidth(pixmap->size().width());//设置默认大小
            setImplicitHeight(pixmap->size().height());

            if( width()>0 )
                onWidthChanged();
            if(height ()>0)
                onHeightChanged();
            update ();
        }else{
            emit loadError ();
        }
        if(m_source != arg)
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
        bitmap->load (str);
        emit maskSourceChanged(arg);
    }
}

void MyImage::setCache(bool arg)
{
    if (m_cache != arg) {
        m_cache = arg;
        emit cacheChanged(arg);
    }
}

void MyImage::setGrayscale(bool arg)
{
    if(m_grayscale!=arg){
        m_grayscale = arg;
        bool old_cache = cache ();
        m_cache = false;
        setSource (source ());
        m_cache = old_cache;
        emit grayscaleChanged(arg);
    }
}
