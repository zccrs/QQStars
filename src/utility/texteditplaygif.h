#ifndef TEXTEDITPLAYGIF_H
#define TEXTEDITPLAYGIF_H

#include <QObject>
#include <QQuickTextDocument>
#include <QList>
#include <QThread>
#include <QPointer>
#include <QUrl>

class QQuickTextEdit;
class TextEditPlayGif : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QQuickTextEdit* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QUrl cachePath READ cachePath WRITE setCachePath NOTIFY cachePathChanged)
public:
    TextEditPlayGif(QObject *parent=0);
    
    QQuickTextEdit* target() const;
    QUrl cachePath() const;
private:
    struct MovieData{
        QMovie* movie;
        QString url;
        QString gifName;
        QSize size;
    };
    
    QPointer<QQuickTextEdit> m_target;
    QList<MovieData> list_movie;
    QStringList list_errorUrl;//记录那些解析出错的gif的路径
    QUrl m_cachePath;
    QString old_content;
    
    void clearMovie();
    void addMovie(QMovie *movie, const QString &url, const QString& gif_name, QSize size);
    QString getUrlByMovie(QMovie* movie);
    QString getGifNameByMovie(QMovie* movie);
    QMovie* getMovieByUrl(const QString& url);
    MovieData* getMovieDataByMovie(const QMovie* movie);
    void setUrlByMovie(QMovie* movie, const QString &url);
    
    bool isErrorUrl(const QString url);
    void addErrorUrl(const QString url);
public slots:
    void onTextChanged();
private slots:
    void onMovie(int frame);
    void onMovieFinished();
signals:
    void targetChanged(QQuickTextEdit* arg);
    void cachePathChanged(QUrl arg);
    void error(const QString& errorString);
public slots:
    void setTarget(QQuickTextEdit* arg);
    void setCachePath(QUrl arg);
    
    void removeErrorUrl(const QString& url);
};

#endif // TEXTEDITPLAYGIF_H
