#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QQuickWindow>
#include <QPoint>
#include <QQuickItem>
#include <QBackingStore>
#include <QPixmap>

class MyWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QUrl windowIcon READ windowIcon WRITE setWindowIcon NOTIFY windowIconChanged)
    Q_PROPERTY(bool noBorder READ noBorder WRITE setNoBorder NOTIFY noBorderIconChanged)
    Q_PROPERTY(Status windowStatus READ windowStatus WRITE setWindowStatus NOTIFY windowStatusChanged)
    Q_PROPERTY(int borderLeft READ borderLeft)
    Q_PROPERTY(int borderRight READ borderRight)
    Q_PROPERTY(int borderTop READ borderTop)
    Q_PROPERTY(bool topHint READ topHint WRITE setTopHint NOTIFY topHintChanged)//窗体保持在最前端
    Q_PROPERTY(bool noNotifyIcon READ noNotifyIcon WRITE setNoNotifyIcon NOTIFY noNotifyIconChanged)//无状态栏图标
    Q_ENUMS(Status)
public:
    explicit MyWindow(QQuickWindow *parent = 0);
    enum Status{
        StopCenter,//初始状态,停在屏幕中，不靠任何边界
        BerthPrepare,//进入准停靠状态（此时鼠标离开窗体就会停靠）
        BerthLeft,//停靠在左边
        BerthRight,//停靠在右边
        BerthTop//停靠在上边
    };
    
    bool noNotifyIcon() const
    {
        return m_noNotifyIcon;
    }
    
protected:
    /*bool event(QEvent * ev);
    void resizeEvent(QResizeEvent *event);
    void exposeEvent(QExposeEvent *event);*/
private:
    QBackingStore *backingStore;
    QUrl m_windowIcon;
    QUrl windowIcon();
    void setWindowIcon( QUrl icon );
    bool m_noBorder;
    Status m_windowStatus;
    bool m_topHint, old_topHint;
    bool m_noNotifyIcon;
    QPixmap pixmap;
private slots:
    void paint();
signals:
    void windowIconChanged();
    void noBorderIconChanged();
    void windowStatusChanged();
    void topHintChanged(bool arg);
    void noNotifyIconChanged(bool arg);
    
public slots:
    bool noBorder();
    void setNoBorder( bool isNoBroder );
    
    Status windowStatus();
    void setWindowStatus( Status new_status );
    
    int borderLeft();
    int borderRight();
    int borderTop();
    
    bool topHint() const;
    void setTopHint(bool arg);
    
    void setNoNotifyIcon(bool arg);
};

#endif // MYWINDOW_H
