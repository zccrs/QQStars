#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QQuickItem>
#include <QShortcut>
#include "qxtglobalshortcut.h"

class MenuPrivate: public QMenu
{
    Q_OBJECT
public:
    explicit MenuPrivate(QWidget *parent = 0);
    explicit MenuPrivate(const QString &title, QWidget *parent = 0);
};
class MyMenuItem : public QAction
{
    Q_OBJECT
    Q_PROPERTY(QUrl icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
    QUrl m_myIcon;
    QxtGlobalShortcut* m_shortcut;
public:
    explicit MyMenuItem(QObject *parent = 0);
    QUrl icon() const;
    QString shortcut() const;
public slots:
    void setIcon(QUrl icon);
    void setShortcut(const QString &shortcut);
signals:
    void iconChanged();
    void shortcutChanged(QString arg);
};
class MenuSeparator : public QObject
{
public:
    explicit MenuSeparator(QObject *parent=0);
};
class SystemTrayIcon;
class MyMenu : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl styleSource READ styleSource WRITE setStyleSource NOTIFY styleSourceChanged)
    Q_PROPERTY(QString styleSheet READ styleSheet WRITE setStyleSheet NOTIFY styleSheetChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
    MenuPrivate *menu;
    QUrl m_styleSource;
    
protected:
    void componentComplete();    
public:
    explicit MyMenu(QQuickItem *parent = 0);
    
    QUrl styleSource() const
    {
        return m_styleSource;
    }
    
    int width() const
    {
        return menu->width ();
    }
    
    int height() const
    {
        return menu->height ();
    }
    
    QString styleSheet() const
    {
        return menu->styleSheet ();
    }
    
public slots:
    void clear();
    void addSeparator ();
    void addMenuItem(MyMenuItem *item);
    void addMenu( MyMenu *mymenu);
    friend class SystemTrayIcon;
    void setStyleSource(QUrl arg);
    
    void popup();
    void setWidth(int arg)
    {
        int m_width = menu->width ();
        if (m_width != arg) {
            menu->setFixedWidth (arg);
            emit widthChanged(arg);
        }
    }
    
    void setHeight(int arg)
    {
        int m_height = menu->height ();
        if (m_height != arg) {
            menu->setFixedHeight (arg);
            emit heightChanged(arg);
        }
    }
    
    void setStyleSheet(QString arg)
    {
        QString m_styleSheet = menu->styleSheet ();
        if (m_styleSheet != arg) {
            menu->setStyleSheet (arg);
            emit styleSheetChanged(arg);
        }
    }
    
signals:
    void styleSourceChanged(QUrl arg);
    void widthChanged(int arg);
    void heightChanged(int arg);
    void styleSheetChanged(QString arg);
};

class SystemTrayIcon : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl windowIcon READ windowIcon WRITE setWindowIcon NOTIFY windowIconChanged)
    Q_PROPERTY(MyMenu* menu READ menu WRITE setMenu NOTIFY menuChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_ENUMS(ActivationReason)
    QUrl m_windowIcon;
    MyMenu* m_menu;
    QSystemTrayIcon systempTray;
    QString m_toolTip;
    
protected:
    
public:
    explicit SystemTrayIcon(QQuickItem *parent = 0);
    QUrl windowIcon() const;
    
    enum ActivationReason {
        Unknown,
        Context,
        DoubleClick,
        Trigger,
        MiddleClick
    };
    
    QString toolTip() const
    {
        return m_toolTip;
    }
    
private slots:
    void onActivated( QSystemTrayIcon::ActivationReason reason );
    void onVisibleChanged();
MyMenu* menu() const
{
    return m_menu;
}

signals:
    void windowIconChanged();
    void menuChanged(MyMenu* arg);
    void activated(ActivationReason arg);
    void toolTipChanged(QString arg);
    void messageClicked();
public slots:
    void setWindowIcon(QUrl icon);
    void showMessage(const QString & title, const QString & message, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int millisecondsTimeoutHint = 10000);
void setMenu(MyMenu* arg)
{
    if (m_menu != arg) {
        m_menu = arg;
        systempTray.setContextMenu (m_menu->menu);
        emit menuChanged(arg);
    }
}
void setToolTip(QString arg)
{
    if (m_toolTip != arg) {
        m_toolTip = arg;
        systempTray.setToolTip (arg);
        emit toolTipChanged(arg);
    }
}
};

#endif // SYSTEMTRAYICON_H
