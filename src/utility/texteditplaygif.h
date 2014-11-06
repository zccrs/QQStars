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
    Q_PROPERTY(QUrl cachePath READ cachePath WRITE setCachePath NOTIFY cachePathChanged)//此属性暂时未使用
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    
public:
    TextEditPlayGif(QObject *parent=0);
    
    QQuickTextEdit* target() const;
    QUrl cachePath() const;
    bool enabled() const;
    
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
    bool m_enabled;
    
    void clearMovie();
    void addMovie(QMovie *movie, const QString &url, const QString& gif_name, QSize size);
    QString getUrlByMovie(QMovie* movie);
    QString getGifNameByMovie(QMovie* movie);
    MovieData* getDataByGifNameAndSize(const QString& name, QSize size);
    MovieData* getDataByMovie(const QMovie* movie);
    void setUrlByMovie(QMovie* movie, const QString &url);
    
    bool isErrorUrl(const QString url);
    void addErrorUrl(const QString url);
    
    void startAllMovie();
    void stopAllMovie();
    
    void setTextEditContent(const QString& data);
public slots:
    void onTextChanged();
private slots:
    void onMovie(int frame);
    void onMovieFinished();
signals:
    void targetChanged(QQuickTextEdit* arg);
    void cachePathChanged(QUrl arg);
    void error(const QString& errorString);
    void enabledChanged(bool arg);
    
public slots:
    void setTarget(QQuickTextEdit* arg);
    void setCachePath(QUrl arg);
    
    void removeErrorUrl(const QString& url);
    void setEnabled(bool arg);
};

#endif // TEXTEDITPLAYGIF_H
