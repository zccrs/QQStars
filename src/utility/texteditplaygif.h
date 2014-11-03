#ifndef TEXTEDITPLAYGIF_H
#define TEXTEDITPLAYGIF_H

#include <QObject>
#include <QQuickTextDocument>
#include <QList>
#include <QThread>

class TextEditPlayGif;
class TextEditPlayGifPrivate : public QObject
{
    Q_OBJECT
private:
    TextEditPlayGifPrivate();
public slots:
    void setDocHtml(QTextDocument *doc, const QString& data);
    
    friend class TextEditPlayGif;
};

class TextEditPlayGif : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QUrl cachePath READ cachePath WRITE setCachePath NOTIFY cachePathChanged)
    
public:
    explicit TextEditPlayGif(QObject *parent = 0);
    ~TextEditPlayGif();
    QQuickTextDocument* target() const;
    QUrl cachePath() const;
    
private:
    struct MovieData{
        QMovie* movie;
        QString url;
        QString gifName;
    };
    QQuickTextDocument* m_target;
    QTextDocument *doc;
    QList<MovieData> list_movie;
    QString doc_content;
    QUrl m_cachePath;
    QThread* mythread;
    
    void clearMovie();
    void addMovie(QMovie *movie, const QString &url, const QString& gif_name);
    QString getUrlByMovie(QMovie* movie);
    QString getGifNameByMovie(QMovie* movie);
    QMovie* getMovieByUrl(const QString& url);
    void setUrlByMovie(QMovie* movie, const QString &url);

private slots:
    void onTextChanged();
    void onMovie(int frame);
    void onMovieFinished();
signals:
    void targetChanged(QQuickTextDocument* arg);
    void cachePathChanged(QUrl arg);
    void setDocHtml(QTextDocument *doc, QString data);
public slots:
    void setTarget(QQuickTextDocument* arg);
    void setCachePath(QUrl arg);
};

#endif // TEXTEDITPLAYGIF_H
