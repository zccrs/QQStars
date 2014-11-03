#ifndef TEXTEDITPLAYGIF_H
#define TEXTEDITPLAYGIF_H

#include <QObject>
#include <QQuickTextDocument>
#include <QList>

class TextEditPlayGif : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target READ target WRITE setTarget NOTIFY targetChanged)
    
    struct MovieData{
        QMovie* movie;
        QString url;
        QString gifName;
    };
    QQuickTextDocument* m_target;
    QTextDocument *doc;
    QList<MovieData> list_movie;
    QString doc_content;
public:
    explicit TextEditPlayGif(QObject *parent = 0);
    ~TextEditPlayGif();
    QQuickTextDocument* target() const;
private:
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
public slots:
    void setTarget(QQuickTextDocument* arg);
};

#endif // TEXTEDITPLAYGIF_H
