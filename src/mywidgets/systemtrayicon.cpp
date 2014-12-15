#include "systemtrayicon.h"
#include <QDebug>
#include <QKeySequence>
#include <QFile>
#include "utility.h"
#include "qxtglobalshortcut.h"

SystemTrayIcon::SystemTrayIcon(QQuickItem *parent) :
    QQuickItem(parent)
{
    setVisible (false);
    systempTray = new QSystemTrayIcon(this);
    connect (systempTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    connect (systempTray, SIGNAL(messageClicked()), this, SIGNAL(messageClicked()));
    connect (this, SIGNAL(visibleChanged()), SLOT(onVisibleChanged()));
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    emit activated (ActivationReason(reason));
}

void SystemTrayIcon::onVisibleChanged()
{
    systempTray->setVisible (isVisible ());
}

MyMenu *SystemTrayIcon::menu() const
{
    return m_menu;
}

QUrl SystemTrayIcon::windowIcon() const
{
    return m_windowIcon;
}

QString SystemTrayIcon::toolTip() const
{
    return m_toolTip;
}

int SystemTrayIcon::x() const
{
    return systempTray->geometry ().x ();
}
int SystemTrayIcon::y() const
{
    return systempTray->geometry ().y ();
}

int SystemTrayIcon::width() const
{
    return systempTray->geometry ().width ();
}

int SystemTrayIcon::height() const
{
    return systempTray->geometry ().height ();
}
void SystemTrayIcon::setWindowIcon(QUrl icon)
{
    if( icon!=m_windowIcon ){
        QString str = icon.toLocalFile();
        if(str == ""){
            str = icon.toString();
        }

        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        systempTray->setIcon (QIcon(str));
        m_windowIcon = icon;
        emit windowIconChanged ();
    }
}

void SystemTrayIcon::showMessage(const QString &title, const QString &message, QSystemTrayIcon::MessageIcon icon, int millisecondsTimeoutHint)
{
    systempTray->showMessage (title, message, icon, millisecondsTimeoutHint);
}

void SystemTrayIcon::setMenu(MyMenu *arg)
{
    if (m_menu != arg) {
        m_menu = arg;
        systempTray->setContextMenu (m_menu->menu);
        emit menuChanged(arg);
    }
}

void SystemTrayIcon::setToolTip(QString arg)
{
    if (m_toolTip != arg) {
        m_toolTip = arg;
        systempTray->setToolTip (arg);
        emit toolTipChanged(arg);
    }
}

MyMenuItem::MyMenuItem(QObject *parent) : 
    QAction(parent)
{
    setObjectName ("MyMenuItem");
    m_shortcut = new QxtGlobalShortcut();
    connect(m_shortcut, SIGNAL(activated()), SLOT(trigger()));
}

QUrl MyMenuItem::icon() const
{
    return m_myIcon;
}
QString MyMenuItem::shortcut() const
{
    return m_shortcut->shortcut ().toString ();
}

void MyMenuItem::setIcon(QUrl icon)
{
    if( icon!=m_myIcon ){
        QString str = icon.toLocalFile();
        if(str == ""){
            str = icon.toString();
        }

        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        QAction::setIcon (QIcon(str));
        m_myIcon = icon;
        emit iconChanged ();
    }
}

void MyMenuItem::setShortcut(const QString &shortcut)
{
    if( m_shortcut->setShortcut (QKeySequence(shortcut)) ){
        QAction::setShortcut (QKeySequence(shortcut));
        emit shortcutChanged (shortcut);
    }else{
        qDebug()<<"设置"+shortcut+"热键出错";
        emit shortcutChanged ("set shortcut error");
    }
}

void MyMenu::componentComplete()
{
    QObjectList temp_list = children ();
    for (int i=0; i<temp_list.count (); ++i) {
        QObject *obj = temp_list.at (i);
        if( obj->objectName ()=="MyMenuItem"){
            MyMenuItem *item = qobject_cast<MyMenuItem*>(obj);
            menu->addAction(item);
        }else if(obj->objectName ()=="MenuSeparator"){
            menu->addSeparator ();
        }else if(obj->objectName ()=="MyMenu"){
            MyMenu *item = qobject_cast<MyMenu*>(obj);
            menu->addMenu (item->menu);
        }
    }
}

MyMenu::MyMenu(QQuickItem *parent) : 
    QQuickItem(parent)
{
    m_styleSource = QUrl("");
    setObjectName ("MyMenu");
    menu = new MenuPrivate();
    setVisible (false);
}

QUrl MyMenu::styleSource() const
{
    return m_styleSource;
}

int MyMenu::width() const
{
    return menu->width ();
}

int MyMenu::height() const
{
    return menu->height ();
}

QString MyMenu::styleSheet() const
{
    return menu->styleSheet ();
}

void MyMenu::clear()
{
    menu->clear ();
}

void MyMenu::addSeparator()
{
    menu->addSeparator ();
}

void MyMenu::addMenuItem(MyMenuItem *item)
{
    menu->addAction(item);
}

void MyMenu::addMenu(MyMenu *mymenu)
{
    menu->addMenu (mymenu->menu);
}

void MyMenu::setStyleSource(QUrl arg)
{
    if (m_styleSource != arg) {
        m_styleSource = arg;

        QString str = arg.toLocalFile();
        if(str == ""){
            str = arg.toString();
            if(str==""){
                return;
            }
        }

        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);

        QFile file(str);
        
        if(file.open (QIODevice::ReadOnly)){
            menu->setStyleSheet (file.readAll ());
        }else{
            qDebug()<<"打开"+str+"失败；"<<file.errorString ();
        }
        file.close ();
        emit styleSourceChanged(arg);
    }
}

void MyMenu::popup()
{
    QPoint pos = Utility::createUtilityClass ()->mouseDesktopPos();
    menu->move (pos);
    menu->show ();
}

void MyMenu::setWidth(int arg)
{
    int m_width = menu->width ();
    if (m_width != arg) {
        menu->setFixedWidth (arg);
        emit widthChanged(arg);
    }
}

void MyMenu::setHeight(int arg)
{
    int m_height = menu->height ();
    if (m_height != arg) {
        menu->setFixedHeight (arg);
        emit heightChanged(arg);
    }
}

void MyMenu::setStyleSheet(QString arg)
{
    QString m_styleSheet = menu->styleSheet ();
    if (m_styleSheet != arg) {
        menu->setStyleSheet (arg);
        emit styleSheetChanged(arg);
    }
}


MenuSeparator::MenuSeparator(QObject *parent):
    QObject(parent)
{
    setObjectName ("MenuSeparator");
}

MenuPrivate::MenuPrivate(QWidget *parent):
    QMenu(parent)
{
}

MenuPrivate::MenuPrivate(const QString &title, QWidget *parent):
    QMenu(title, parent)
{
}
