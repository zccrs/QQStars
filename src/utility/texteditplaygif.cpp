#include "texteditplaygif.h"
#include <QRegExp>
#include <QLabel>
#include <QMovie>
#include <QTextImageFormat>
#include <QTextCursor>
#include <QDebug>
#include <QDir>
#include <private/qquicktextedit_p.h>
#include <QtConcurrent>

TextEditPlayGif::TextEditPlayGif(QObject *parent) :
    QObject(parent)
{
}

QQuickTextEdit *TextEditPlayGif::target() const
{
    return m_target.data ();
}

QUrl TextEditPlayGif::cachePath() const
{
    return m_cachePath;
}

void TextEditPlayGif::setTarget(QQuickTextEdit *arg)
{
    if (m_target == arg)
        return;
    
    disconnect (m_target, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
    m_target = arg;
    connect (m_target, SIGNAL(textChanged()), SLOT(onTextChanged()));
    emit targetChanged(arg);
}

void TextEditPlayGif::setCachePath(QUrl arg)
{
    if (m_cachePath == arg)
        return;
    
    m_cachePath = arg;
    emit cachePathChanged(arg);
}

void TextEditPlayGif::removeErrorUrl(const QString &url)
{
    list_errorUrl.removeOne (url);
}

void TextEditPlayGif::clearMovie()
{
    foreach (MovieData data, list_movie) {
        data.movie->deleteLater ();
    }
    list_movie.clear ();
}

void TextEditPlayGif::addMovie(QMovie *movie, const QString &url, const QString &gif_name, QSize size)
{
    MovieData data;
    data.movie=movie;
    data.url=url;
    data.gifName=gif_name;
    data.size=size;
    list_movie<<data;
}

QString TextEditPlayGif::getUrlByMovie(QMovie *movie)
{
    foreach (MovieData data, list_movie) {
        if(data.movie==movie)
            return data.url;
    }
    return "";
}

QString TextEditPlayGif::getGifNameByMovie(QMovie *movie)
{
    foreach (MovieData data, list_movie) {
        if(data.movie==movie)
            return data.gifName;
    }
    return "";
}

QMovie *TextEditPlayGif::getMovieByUrl(const QString &url)
{
    foreach (MovieData data, list_movie) {
        if(data.url==url)
            return data.movie;
    }
    return NULL;
}

TextEditPlayGif::MovieData *TextEditPlayGif::getMovieDataByMovie(const QMovie *movie)
{
    for(int i=0;i<list_movie.size ();++i){
        MovieData* data = &list_movie[i];
        if(data->movie==movie)
            return data;
    }
    return NULL;
}

void TextEditPlayGif::setUrlByMovie(QMovie *movie, const QString &url)
{
    for(int i=0;i<list_movie.size ();++i){
        MovieData* data = &list_movie[i];
        if(data->movie==movie)
            data->url=url;
    }
}

bool TextEditPlayGif::isErrorUrl(const QString url)
{
    foreach (QString str, list_errorUrl) {
        if(str==url){
            return true;
        }
    }
    return false;
}

void TextEditPlayGif::addErrorUrl(const QString url)
{
    if(!isErrorUrl (url))
        list_errorUrl<<url;
}

void TextEditPlayGif::onTextChanged()
{
    QString content = m_target->text ();
    if((content==old_content))
        return;
    old_content = content;
    //qDebug()<<content;
    
    QRegExp reg("<img src=\"[^\"]+.gif\".+/>");
    reg.setMinimal (true);
    reg.indexIn (content);
    
    foreach (QString img, reg.capturedTexts ()) {
        if(img=="")
            break;
        //qDebug()<<img;
        QRegExp reg("src=\".+\"");
        reg.setMinimal (true);
        
        QRegExp reg_width("width=\"\\d+\"");
        reg_width.setMinimal (true);
        int width=-1;
        if(reg_width.indexIn (img)>=0){//如果设置了width
            QString width_str = reg_width.cap (0);
            width_str.replace ("width=\"","");
            width_str = width_str.left (width_str.size ()-1);
            width = width_str.toInt ();
        }
        QRegExp reg_height("height=\"\\d+\"");
        reg_height.setMinimal (true);
        int height=-1;
        if(reg_height.indexIn (img)>=0){//如果设置了width
            QString height_str = reg_height.cap (0);
            height_str.replace ("height=\"","");
            height_str = height_str.left (height_str.size ()-1);
            height = height_str.toInt ();
        }
        
        if(reg.indexIn (img)>=0){//如果src存在
            QString src = reg.cap (0);
            if(src=="")
                break;
            img = src;
            src = src.mid (5, src.size ()-6);
            QString gifName="";
            foreach (QChar ch, src) {
                if(ch.isLetter ()||ch.isNumber ())
                    gifName.append (ch);
            }
            
            if(src.left (3)=="qrc")
                src=src.mid (3);
            if(isErrorUrl (src))
                break;
            QImage image;
            if(image.load (src)){
                if(width==-1)
                    width = image.width ();
                if(height==-1)
                    height = image.height ();
            }else{
                addErrorUrl (src);
                break;
            }
            //qDebug()<<width<<height;
            
            QMovie *movie=new QMovie(src, "", this);
            movie->setCacheMode(QMovie::CacheAll);
            addMovie (movie, img, gifName, QSize(width, height));
            connect (movie, SIGNAL(frameChanged(int)), SLOT(onMovie(int)));
            connect (movie, SIGNAL(finished()), SLOT(onMovieFinished()));
            movie->start ();
        }
    }
}

void saveImage(const QString& name, QImage image)
{
    image.save (name);
}

void TextEditPlayGif::onMovie(int frame)
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    MovieData* movie_data = getMovieDataByMovie (movie);
    
    if(movie&&movie_data&&movie_data->url!=""){
        QString name = QDir::homePath ()+"/webqq/cacheImage/"+movie_data->gifName+"/";
        QDir dir;
        if(dir.mkpath (name)){//创建路径
            QImage image = movie->currentImage ();
            name.append (QString::number (frame)+
                         QString::number (movie_data->size.width ())+
                         QString::number (movie_data->size.height ())+
                         ".png");
                
            if(QFile::exists (name)){//如果存在
                name = "src=\"file:///"+name+"\"";
                //qDebug()<<name;
                QString data = m_target->text ().replace (movie_data->url, name);
                //qDebug()<<m_target->text ()<<data;
                setUrlByMovie (movie, name);
                int pos = m_target->cursorPosition ();
                int select_start = m_target->selectionStart ();
                int select_end = m_target->selectionEnd ();
                m_target->setText (data);
                m_target->setCursorPosition (pos);
                m_target->select (select_start, select_end);
            }else{
                image = image.scaled (movie_data->size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                QtConcurrent::run(saveImage, name, image);
            }
        }else{
            qDebug()<<"TextEditPlayGif:路径"<<name<<"创建失败";
        }
    }
}

void TextEditPlayGif::onMovieFinished()
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    if(movie!=NULL&&movie->currentFrameNumber ()!=-1)
        movie->start ();
}
