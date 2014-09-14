#include "systemtrayicon.h"
#include <QDebug>
#include <QKeySequence>
#include <QFile>
#include <QGraphicsDropShadowEffect>
#include "utility.h"

SystemTrayIcon::SystemTrayIcon(QQuickItem *parent) :
    QQuickItem(parent)
{
    setVisible (false);
    connect (&systempTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
    connect (this, SIGNAL(visibleChanged()), SLOT(onVisibleChanged()));
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    emit activated (ActivationReason(reason));
}

void SystemTrayIcon::onVisibleChanged()
{
    systempTray.setVisible (isVisible ());
}

QUrl SystemTrayIcon::windowIcon() const
{
    return m_windowIcon;
}
void SystemTrayIcon::setWindowIcon(QUrl icon)
{
    if( icon!=m_windowIcon ){
        QString str = icon.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        systempTray.setIcon (QIcon(str));
        m_windowIcon = icon;
        emit windowIconChanged ();
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
        QString str = icon.toString ();
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
    setObjectName ("MyMenu");
    menu = new MenuPrivate();
    //QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    //shadow_effect->setOffset(0, 0);
    //shadow_effect->setColor(Qt::black);
    //shadow_effect->setBlurRadius(15);
    //menu->setGraphicsEffect (shadow_effect);
    setVisible (false);
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
        QFile file(arg.toLocalFile ());
        if(file.open (QIODevice::ReadOnly)){
            menu->setStyleSheet (file.readAll ());
        }else{
            qDebug()<<"打开"+arg.toLocalFile ()+"失败；"<<file.errorString ();
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
