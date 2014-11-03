#include "texteditplaygif.h"
#include <QRegExp>
#include <QLabel>
#include <QMovie>
#include <QTextImageFormat>
#include <QTextCursor>
#include <QDebug>
#include <QDir>

TextEditPlayGif::TextEditPlayGif(QObject *parent) :
    QObject(parent)
{
    doc = NULL;
}

TextEditPlayGif::~TextEditPlayGif()
{
    clearMovie ();
}

QQuickTextDocument *TextEditPlayGif::target() const
{
    return m_target;
}

QUrl TextEditPlayGif::cachePath() const
{
    return m_cachePath;
}

void TextEditPlayGif::clearMovie()
{
    foreach (MovieData data, list_movie) {
        data.movie->deleteLater ();
    }
    list_movie.clear ();
}

void TextEditPlayGif::addMovie(QMovie *movie, const QString& url, const QString& gif_name)
{
    MovieData data;
    data.movie=movie;
    data.url=url;
    data.gifName=gif_name;
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

void TextEditPlayGif::setUrlByMovie(QMovie *movie, const QString &url)
{
    for(int i=0;i<list_movie.size ();++i){
        MovieData* data = &list_movie[i];
        if(data->movie==movie)
            data->url=url;
    }
}


void TextEditPlayGif::onTextChanged()
{
    QString content = doc->toHtml ();
    qDebug()<<content;
    QRegExp reg("<img.+src=\"[^\"]+\\.gif\".+/>");
    reg.setMinimal (true);
    reg.indexIn (content);
    //qDebug()<<reg.capturedTexts ();
    foreach (QString img, reg.capturedTexts ()) {
        if(img=="")
            break;
        
        QRegExp reg("src=\".+\"");
        reg.setMinimal (true);
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
            QMovie *movie=new QMovie(src);
            movie->setCacheMode(QMovie::CacheNone);
            addMovie (movie, img, gifName);
            connect (movie, SIGNAL(frameChanged(int)), SLOT(onMovie(int)));
            connect (movie, SIGNAL(finished()), SLOT(onMovieFinished()));
            movie->start ();
        }
    }
}

void TextEditPlayGif::onMovie(int index)
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    QString url = getUrlByMovie (movie);
    if(movie&&url!=""){
        QString name = m_cachePath.toLocalFile ()+"/"+getGifNameByMovie (movie)+"/";
        QDir dir;
        if(dir.mkpath (name)){//创建路径
            name.append (QString::number (index)+".png");
            if(!QFile::exists (name)){//如果不存在
                if(!movie->currentImage ().save (name)){
                    qDebug()<<"TextEditPlayGif:图片保存失败";
                    return;
                }
            }
            name = "src=\"file:///"+name+"\"";
            setUrlByMovie (movie, name);
            QString data = doc->toHtml ().replace (url, name);
            doc->setHtml (data);
        }else{
            qDebug()<<"TextEditPlayGif:路径"<<name<<"创建失败";
        }
    }
}

void TextEditPlayGif::onMovieFinished()
{
    QMovie* movie = qobject_cast<QMovie*>(sender());
    //qDebug()<<movie->currentFrameNumber ();
    if(movie!=NULL&&movie->currentFrameNumber ()!=-1)
        movie->start ();
}

void TextEditPlayGif::setTarget(QQuickTextDocument *arg)
{
    if (m_target == arg)
        return;
    disconnect (doc, &QTextDocument::contentsChanged, this, &TextEditPlayGif::onTextChanged);
    doc = arg->textDocument ();
    //doc_content = doc->toHtml ();
    connect (doc, SIGNAL(contentsChanged()), SLOT(onTextChanged()));
    m_target = arg;
    emit targetChanged(arg);
}

void TextEditPlayGif::setCachePath(QUrl arg)
{
    if (m_cachePath == arg)
        return;
    
    m_cachePath = arg;
    emit cachePathChanged(arg);
}
