#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QUrl>
#include <QMenu>
#include <QAction>
#include <QQuickItem>
#include <QShortcut>

class QxtGlobalShortcut;
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
    
    QUrl styleSource() const;
    int width() const;
    int height() const;
    QString styleSheet() const;
public slots:
    void clear();
    void addSeparator ();
    void addMenuItem(MyMenuItem *item);
    void addMenu( MyMenu *mymenu);
    friend class SystemTrayIcon;
    void setStyleSource(QUrl arg);
    
    void popup();
    void setWidth(int arg);
    void setHeight(int arg);
    void setStyleSheet(QString arg);
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
    Q_PROPERTY(int x READ x CONSTANT)
    Q_PROPERTY(int y READ y CONSTANT)
    Q_PROPERTY(int width READ width CONSTANT)
    Q_PROPERTY(int height READ height CONSTANT)
    Q_ENUMS(ActivationReason)
    QUrl m_windowIcon;
    MyMenu* m_menu;
    QSystemTrayIcon *systempTray;
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
    
    QString toolTip() const;
    int x() const;
    int y() const;
    int width() const;
    int height() const;
private slots:
    void onActivated( QSystemTrayIcon::ActivationReason reason );
    void onVisibleChanged();
    MyMenu* menu() const;

signals:
    void windowIconChanged();
    void menuChanged(MyMenu* arg);
    void activated(ActivationReason arg);
    void toolTipChanged(QString arg);
    void messageClicked();
public slots:
    void setWindowIcon(QUrl icon);
    void showMessage(const QString & title, const QString & message, QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int millisecondsTimeoutHint = 10000);
    void setMenu(MyMenu* arg);
    void setToolTip(QString arg);
};

#endif // SYSTEMTRAYICON_H
