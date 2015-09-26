#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <QObject>
#include <QQuickWindow>
#include <QPoint>
#include <QQuickItem>
#include <QPixmap>
#include <QQueue>

class MyWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QUrl windowIcon READ windowIcon WRITE setWindowIcon NOTIFY windowIconChanged)//状态栏图标
    Q_PROPERTY(bool noBorder READ noBorder WRITE setNoBorder NOTIFY noBorderIconChanged)//无边框
    Q_PROPERTY(WindowStatus windowStatus READ windowStatus WRITE setWindowStatus NOTIFY windowStatusChanged)//窗口状态
    Q_PROPERTY(int borderLeft READ borderLeft CONSTANT)//离屏幕左边的距离
    Q_PROPERTY(int borderRight READ borderRight CONSTANT)//离屏幕右边的距离
    Q_PROPERTY(int borderTop READ borderTop CONSTANT)//离屏幕上边的距离
    Q_PROPERTY(bool topHint READ topHint WRITE setTopHint NOTIFY topHintChanged)//窗体保持在最前端
    Q_PROPERTY(bool noNotifyIcon READ noNotifyIcon WRITE setNoNotifyIcon NOTIFY noNotifyIconChanged)//无状态栏图标
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)//窗口的width（不包含边框的阴影）
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)//窗口的height（不包含边框的阴影）
    Q_PROPERTY(int actualWidth READ actualWidth WRITE setActualWidth NOTIFY actualWidthChanged)//真实的width，包含阴影
    Q_PROPERTY(int actualHeight READ actualHeight WRITE setActualHeight NOTIFY actualHeightChanged)//真实的height，包含阴影
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)//窗口内容相对于桌面的绝对坐标（不包含阴影部分）
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)//窗口内容相对于桌面的绝对坐标（不包含阴影部分）
    Q_PROPERTY(int actualX READ actualX WRITE setActualX NOTIFY actualXChanged)//窗口相对于桌面的绝对坐标（包含阴影部分）
    Q_PROPERTY(int actualY READ actualY WRITE setActualY NOTIFY actualYChanged)//窗口相对于桌面的绝对坐标（包含阴影部分）
    Q_PROPERTY(bool windowActive READ windowActive NOTIFY windowActiveChanged FINAL)//窗口是否获得焦点，是否为活跃窗口
    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
    Q_PROPERTY(int minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumHeightChanged)
    Q_PROPERTY(int maximumWidth READ maximumWidth WRITE setMaximumWidth NOTIFY maximumWidthChanged)
    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight NOTIFY maximumHeightChanged)
    Q_PROPERTY(bool mousePenetrate READ mousePenetrate WRITE setMousePenetrate NOTIFY mousePenetrateChanged)
    //是否穿透鼠标
    Q_PROPERTY(QPoint cursorPos READ cursorPos WRITE setCursorPos FINAL)
    Q_ENUMS(WindowStatus)
public:
    explicit MyWindow(QQuickWindow *parent = 0);
    enum WindowStatus{
        StopCenter,//初始状态,停在屏幕中，不靠任何边界
        BerthPrepare,//进入准停靠状态（此时鼠标离开窗体就会停靠）
        BerthLeft,//停靠在左边
        BerthRight,//停靠在右边
        BerthTop//停靠在上边
    };
    
    QUrl windowIcon();
    bool noNotifyIcon() const;
    int width() const;
    int height() const;
    int actualWidth() const;
    int actualHeight() const;
    bool windowActive() const;
    int x() const;
    int y() const;
    int actualX() const;
    int actualY() const;
    int minimumWidth() const;
    int minimumHeight() const;
    int maximumWidth() const;
    int maximumHeight() const;
    WindowStatus windowStatus();
    bool noBorder();
    int borderLeft();
    int borderRight();
    int borderTop();
    
    bool topHint() const;
    bool mousePenetrate() const;
    QPoint cursorPos() const;

private:
    QUrl m_windowIcon;
    bool m_noBorder;
    WindowStatus m_windowStatus;
    bool m_topHint, old_topHint;
    bool m_noNotifyIcon;
    qreal m_width;
    qreal m_height;
    bool m_windowActive;
    int m_minimumWidth;
    int m_minimumHeight;
    int m_maximumWidth;
    int m_maximumHeight;
    bool m_mousePenetrate;
    
    void setWindowActive(bool arg);

protected:
    void focusInEvent(QFocusEvent * ev);
    void focusOutEvent(QFocusEvent * ev);
private slots:
    void onActualXChanged();
    void onActualYChanged();

signals:
    void windowIconChanged();
    void noBorderIconChanged();
    void windowStatusChanged();
    void topHintChanged(bool arg);
    void noNotifyIconChanged(bool arg);
    
    void widthChanged(int arg);
    void heightChanged(int arg);
    void actualWidthChanged(int arg);
    void actualHeightChanged(int arg);
    void windowActiveChanged(bool arg);
   
    void xChanged();
    void yChanged();
    void actualXChanged(int arg);
    void actualYChanged(int arg);
    void minimumWidthChanged(int arg);
    void minimumHeightChanged(int arg);
    void maximumWidthChanged(int arg);
    void maximumHeightChanged(int arg);
    
    void mousePenetrateChanged(bool arg);
    void closeing();//当调用close时发射;
public slots:
    void setNoBorder( bool isNoBroder );
    void setWindowIcon( QUrl icon );
    void setWindowStatus( WindowStatus new_status );
    void setTopHint(bool arg);
    void setNoNotifyIcon(bool arg);
    void setWidth(int arg);
    void setHeight(int arg);
    void setActualWidth(int arg);
    void setActualHeight(int arg);
    void setX(int arg);
    void setY(int arg);
    void setActualX(int arg);
    void setActualY(int arg);
    void setMinimumWidth(int arg);
    void setMinimumHeight(int arg);
    void setMaximumWidth(int arg);
    void setMaximumHeight(int arg);
    void setMousePenetrate(bool arg);
    void setCursorPos(QPoint cursorPos);
    void setMousePenetrateArea(QRect rect);
    
    void close();
    void deleteWindow();
};

#endif // MYWINDOW_H
