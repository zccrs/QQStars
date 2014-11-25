#include "myimage.h"
#include <QPainter>
#include <QBitmap>
#include <QDir>
#include <QDebug>
#include <QPixmapCache>

#if(QT_VERSION>=0x050000)
MyImage::MyImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
#else
MyImage::MyImage(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
#endif
{
#if(QT_VERSION<0x050000)
    setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
    m_status = Null;
    m_cache = true;
    m_grayscale = false;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(onDownImageFinished(QNetworkReply*)));

    bitmap = new QBitmap;
    connect (this, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect (this, SIGNAL(heightChanged()), SLOT(onHeightChanged()));

    connect(this, SIGNAL(smoothChanged(bool)), SLOT(updatePaintPixmap()));
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

QString MyImage::imageFormatToString(const QByteArray &array)
{
    QByteArray str = array.toHex ();

        if(str.mid (2,6)=="504e47")
            return "png";
        if(str.mid (12,8)=="4a464946")
            return "jpg";
        if(str.left (6)=="474946")
            return "gif";
        if(str.left (4)=="424d")
            return "bmp";
        return "";
}

void MyImage::downloadImage(const QUrl &url)
{
    setStatus(Loading);

    if(reply!=NULL){
        reply->abort();
        //先结束上次的网络请求
    }

    QNetworkRequest request;
    request.setUrl(url);
    reply = manager.get(request);
}

void MyImage::setPixmap(QImage image)
{
    if(image.isNull())
        return;

    if(m_cache){//如果缓存图片
        QPixmapCache::insert(m_source.toString(), QPixmap::fromImage(image));
    }

    if(m_grayscale){//如果为黑白
        image = chromaticToGrayscale(image);//转换为黑白图
    }
    pixmap = QPixmap::fromImage (image);

    if(m_maskSource.toString()!=""&&(!bitmap->isNull())){
        int max_width = bitmap->width();
        int max_height = bitmap->height();
        max_width = pixmap.width()>max_width?pixmap.width():max_width;
        max_height = pixmap.height()>max_height?pixmap.height():max_height;

        pixmap = pixmap.scaled(max_width, max_height);
        pixmap.setMask (bitmap->scaled(max_width, max_height));
    }

    setImplicitWidth(pixmap.size().width());//设置默认大小
    setImplicitHeight(pixmap.size().height());

    if( width()>0 )
        onWidthChanged();
    if(height ()>0)
        onHeightChanged();

    updatePaintPixmap();
    update();
}

void MyImage::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
#if(QT_VERSION>=0x050000)
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#else
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
#endif
    updatePaintPixmap();
}

void MyImage::onWidthChanged()
{
    setImplicitHeight (pixmap.size ().height ()*(width ()/pixmap.size ().width ()));
}
 
void MyImage::onHeightChanged()
{
    setImplicitWidth (pixmap.size ().width ()*(height ()/pixmap.size ().height ()));
}

void MyImage::onDownImageFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        qDebug()<<QString::fromUtf8("MyImage:图片下载成功");

        QImage image;
        if( !image.loadFromData(reply->readAll())){
            emit loadError ();
            setStatus(Error);
            qDebug()<<QString::fromUtf8("MyImage:图片加载出错");
            return;
        }

        setPixmap(image);
        setStatus(Ready);
    }else{
        setStatus(Error);
        emit loadError();
    }
}

void MyImage::updatePaintPixmap()
{
    if(pixmap.isNull())
        return;

    if(smooth())
        paint_pixmap = pixmap.scaled(boundingRect().size().toSize(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    else
        paint_pixmap = pixmap.scaled(boundingRect().size().toSize(), Qt::IgnoreAspectRatio);
}

#if(QT_VERSION>=0x050000)
void MyImage::paint(QPainter *painter)
#else
void MyImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
#endif
{
    painter->drawPixmap (0, 0, paint_pixmap);
}

MyImage::State MyImage::status() const
{
    return m_status;
}

void MyImage::setSource(QUrl arg)
{
    if (!m_cache||m_source != arg) {
        setStatus(Loading);

        m_source = arg;
        reLoad();
        //加载图片
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
        reLoad();
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
        reLoad();
        emit grayscaleChanged(arg);
    }
}

void MyImage::setStatus(MyImage::State arg)
{
    if (m_status != arg) {
        m_status = arg;
        emit statusChanged(arg);
    }
}

void MyImage::reLoad()
{
    QString str = m_source.toString();

    QPixmap *temp_pximap = QPixmapCache::find(str);
    if(temp_pximap!=NULL){//如果缓存区已经有图片
        setPixmap(temp_pximap->toImage());
        setStatus(Ready);
        return;
    }

    if(str.indexOf("http")==0){//如果是网络图片
        downloadImage(m_source);
        return;
    }

    if( str.mid (0, 3) == "qrc")
        str = str.mid (3, str.count ()-3);

    QImage image;
    if( !image.load (str)){
        emit loadError ();
        setStatus(Error);
        return;
    }

    setPixmap(image);
}
