#include "myimage.h"
#include <QPainter>
#include <QBitmap>
#include <QDir>
#include <QDebug>
#include <QPixmapCache>

QCache<QString, QPixmap> MyImage::pixmapCache;

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
    m_source = "";
    m_sourceSize = QSize(0,0);
    m_defaultSize = QSize(0,0);
    pixmap = NULL;
    reply = NULL;

    connect(&manager, SIGNAL(finished(QNetworkReply*)), SLOT(onDownImageFinished(QNetworkReply*)));
}

MyImage::~MyImage()
{
    if(pixmap!=NULL)
        delete pixmap;
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

void MyImage::chromaticToGrayscale(QImage &image)
{
    if(image.isNull()||image.isGrayscale ())
        return;
    for(int i=0;i<image.width ();++i){
        for(int j=0;j<image.height ();++j){
            QRgb pixel = image.pixel(i,j);
            int a = qAlpha(pixel);
            pixel = qGray (pixel);
            pixel = qRgba(pixel,pixel,pixel,a);
            image.setPixel (i,j,pixel);
        }
    }
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

void MyImage::setImage(QImage &image)
{
    setDefaultSize(image.size());

    if(m_cache){
        QPixmap *temp_pixmap = new QPixmap(QPixmap::fromImage(image));
        pixmapCache.insert(m_source.toString(), temp_pixmap);
    }

    QSize temp_size = m_sourceSize;
    if(temp_size==QSize(0,0)){
        temp_size = m_defaultSize;
    }else if(temp_size.width()==0){
        temp_size.setWidth((double)temp_size.height()/m_defaultSize.height()*m_defaultSize.width());
    }else if(temp_size.height()==0){
        temp_size.setHeight((double)temp_size.width()/m_defaultSize.width()*m_defaultSize.height());
    }
    if(m_defaultSize!=temp_size)
        image = image.scaled(temp_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(m_grayscale){//如果为黑白
        chromaticToGrayscale(image);//转换为黑白图
    }

    if(pixmap!=NULL)
        delete pixmap;
    pixmap = new QPixmap(QPixmap::fromImage(image));

    QString str = m_maskSource.toLocalFile();
    if(str==""){
        str = m_maskSource.toString();
    }
    if(str!=""){
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        QBitmap bitmap(str);
        if(!bitmap.isNull()){
            int max_width = bitmap.width();
            int max_height = bitmap.height();
            max_width = pixmap->width()>max_width?pixmap->width():max_width;
            max_height = pixmap->height()>max_height?pixmap->height():max_height;

            QPixmap temp_pixmap = pixmap->scaled(max_width, max_height);
            temp_pixmap.setMask (bitmap.scaled(temp_pixmap.size()));
            *pixmap = temp_pixmap.scaled(image.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
    }

    setImplicitWidth(image.width());
    setImplicitHeight(image.height());

    update();

    setStatus(Ready);
    emit loadReady();
}

void MyImage::onDownImageFinished(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QImage image;
        if( !image.loadFromData(reply->readAll())){
            emit loadError ();
            setStatus(Error);
            return;
        }
        setImage(image);
    }else{
        setStatus(Error);
        emit loadError();
    }
}

#if(QT_VERSION>=0x050000)
void MyImage::paint(QPainter *painter)
#else
void MyImage::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
#endif
{
    if(pixmap==NULL||pixmap->isNull())
        return;

    if(smooth())
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap (boundingRect().toRect(), *pixmap);
}

MyImage::State MyImage::status() const
{
    return m_status;
}

QSize MyImage::sourceSize() const
{
    return m_sourceSize;
}

QSize MyImage::defaultSize() const
{
    return m_defaultSize;
}

const QPixmap *MyImage::getPixmap() const
{
    return pixmap;
}

void MyImage::setSource(QUrl arg)
{
    if (!m_cache||m_source != arg) {
        setStatus(Loading);
        bool can_emit = m_source==arg;
        m_source = arg;
        reLoad();
        //加载图片
        if(can_emit)
            emit sourceChanged(arg);
    }
}

void MyImage::setMaskSource(QUrl arg)
{
    if (m_maskSource != arg) {
        m_maskSource = arg;
        if(pixmap!=NULL)
            reLoad();
        emit maskSourceChanged(arg);
    }
}

void MyImage::setCache(bool arg)
{
    if (m_cache != arg) {
        m_cache = arg;
        if(!m_cache){
            QPixmap* temp_pixmap = pixmapCache.object(m_source.toString());
            pixmapCache.remove(m_source.toString());
            if(temp_pixmap!=NULL)
                delete temp_pixmap;
        }
        emit cacheChanged(arg);
    }
}

void MyImage::setGrayscale(bool arg)
{
    if(m_grayscale!=arg){
        m_grayscale = arg;
        if(pixmap!=NULL){
            if(m_grayscale){
                QImage image = pixmap->toImage();
                chromaticToGrayscale(image);
                *pixmap = QPixmap::fromImage(image);
                update();
            }else{
                reLoad();
            }
        }
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
    QString str = m_source.toLocalFile();
    if(str == ""){
        str = m_source.toString();
        if(str==""){
            if(pixmap!=NULL){
                delete pixmap;
                pixmap = NULL;
            }
            return;
        }
    }

    QPixmap *temp_pixmap = pixmapCache.object(m_source.toString());
    if(temp_pixmap!=NULL){
        bool temp_cache = m_cache;
        m_cache = false;//先设置false，防止setImage中再次更新缓存
        QImage image = temp_pixmap->toImage();
        setImage(image);
        m_cache = temp_cache;
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

    setImage(image);
}

void MyImage::setSourceSize(QSize arg)
{
    if (m_sourceSize != arg) {
        m_sourceSize = arg;
        if(pixmap!=NULL)
            reLoad();
        emit sourceSizeChanged(arg);
    }
}

void MyImage::setDefaultSize(QSize arg)
{
    if (m_defaultSize != arg) {
        m_defaultSize = arg;
        emit defaultSizeChanged(arg);
    }
}

bool MyImage::save(const QString& fileName) const
{
    if(pixmap!=NULL){
        return pixmap->save(fileName);
    }
    return false;
}
