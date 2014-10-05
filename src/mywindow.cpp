#include "mywindow.h"
#include "utility.h"
#include <QDebug>
#include <QQmlComponent>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#ifdef Q_OS_WIN
#include <winuser.h>
#endif
#include <QWidget>

bool test(QObject *, Qt::ShortcutContext)
{
    return true;
}

MyWindow::MyWindow(QQuickWindow *parent) :
    QQuickWindow(parent)
{
    setObjectName ("MyWindow");
    setActualWidth (QQuickWindow::width ());
    setActualHeight (QQuickWindow::height ());
    
    m_noBorder = false;
    m_windowStatus = StopCenter;
    m_topHint = false;
    old_topHint=false;
    m_noNotifyIcon = false;
    m_windowActive = false;
    m_shortcuts = NULL;
}

bool MyWindow::topHint() const
{
    return m_topHint;
}

QUrl MyWindow::windowIcon()
{
    return m_windowIcon;
}

void MyWindow::setWindowIcon(QUrl icon)
{
    if( icon!=m_windowIcon ){
        QString str = icon.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        setIcon (QIcon(str));
        m_windowIcon = icon;
        emit windowIconChanged ();
    }
}

void MyWindow::setWindowActive(bool arg)
{
    if(arg!=m_windowActive){
        m_windowActive = arg;
        emit windowActiveChanged (arg);
    }
}

void MyWindow::onKeyPressed()
{
    if(m_shortcuts&&queue_key.count ()>0){
        foreach (MyWindowShortcut *shortcut, m_shortcuts->list ()) {
            shortcut->onKeyPressed (queue_key);
        }
    }
}

void MyWindow::focusInEvent(QFocusEvent *ev)
{
    QQuickWindow::focusInEvent(ev);
    setWindowActive (true);
}

void MyWindow::focusOutEvent(QFocusEvent *ev)
{
    QQuickWindow::focusOutEvent (ev);
    setWindowActive (false);
}

void MyWindow::keyPressEvent(QKeyEvent *ev)
{
    QQuickWindow::keyPressEvent (ev);
    queue_key.append (ev->key ());
    //qDebug()<<ev->key ();
    onKeyPressed ();
}

void MyWindow::keyReleaseEvent(QKeyEvent *ev)
{
    QQuickWindow::keyReleaseEvent (ev);
    queue_key.removeOne (ev->key());
}

bool MyWindow::noBorder()
{
    return m_noBorder;
}

void MyWindow::setNoBorder(bool isNoBroder)
{
    if( isNoBroder!=m_noBorder ) {
        m_noBorder = isNoBroder;
        if(!isVisible ()) {
            if( isNoBroder )
                setFlags (flags ()|Qt::FramelessWindowHint);
            else
                setFlags (flags ()&~Qt::FramelessWindowHint);
        }else{
            if( isNoBroder )
                setFlags (flags ()|Qt::FramelessWindowHint);
            else
                setFlags (flags ()&~Qt::FramelessWindowHint);
        }
        
        emit noBorderIconChanged();
    }
}

MyWindow::Status MyWindow::windowStatus()
{
    return m_windowStatus;
}

void MyWindow::setWindowStatus(MyWindow::Status new_status)
{
    if( new_status!=m_windowStatus ) {
        if( new_status == BerthPrepare&&m_windowStatus!=StopCenter ) {
            setTopHint (old_topHint);
        }else if(new_status!=StopCenter&&new_status!=BerthPrepare){
            old_topHint = topHint ();
            setTopHint (true);//设置窗口为最前端
        }
        m_windowStatus = new_status;
        emit windowStatusChanged ();
    }
}

int MyWindow::borderLeft()
{
#ifdef Q_OS_WIN
    return 0;
#elif defined(Q_OS_MACX)
    return 0;
#elif defined(Q_OS_LINUX)
    return 65;
#else
    return 0;
#endif
}

int MyWindow::borderRight()
{
#ifdef Q_OS_WIN
    return QApplication::screens ()[0]->size ().width ();
#elif defined(Q_OS_MACX)
    return QApplication::screens ()[0]->size ().width ();
#elif defined(Q_OS_LINUX)
    return QApplication::screens ()[0]->size ().width ();
#else
    return QApplication::screens ()[0]->size ().width ();
#endif
}

int MyWindow::borderTop()
{
#ifdef Q_OS_WIN
    return 0;
#elif defined(Q_OS_MACX)
    return 0;
#elif defined(Q_OS_LINUX)
    return 25;
#else
    return 0;
#endif
}


void MyWindow::setTopHint(bool arg)
{
    if (m_topHint != arg) {
        m_topHint = arg;
        if( arg ){
            setFlags (flags ()|Qt::WindowStaysOnTopHint);
#ifdef Q_OS_LINUX
            if(isVisible()){
                hide();
                show();
            }
#endif
        }else{
            setFlags (flags ()&~Qt::WindowStaysOnTopHint);
#ifdef Q_OS_LINUX
            if(isVisible()){
                hide();
                show();
            }
#endif
#ifdef Q_OS_WIN
            SetWindowPos ((HWND)this->winId (),HWND_NOTOPMOST,0,0,width(),height(),SWP_NOSIZE|SWP_NOMOVE);
#endif
        }
        emit topHintChanged(arg);
    }
}

void MyWindow::setNoNotifyIcon(bool arg)
{
    if ( m_noNotifyIcon != arg ) {
        m_noNotifyIcon = arg;
        if( arg )
            setFlags (flags ()&~Qt::Tool);
        else{
            setFlags (flags ()|Qt::Tool);
        }
        emit noNotifyIconChanged(arg);
    }
}

void MyWindow::setWidth(int arg)
{
    if (m_width != arg) {
        m_width = arg;
        emit widthChanged(arg);
    }
    contentItem ()->setWidth (arg);
}

void MyWindow::setHeight(int arg)
{
    if (m_height != arg) {
        m_height = arg;
        emit heightChanged(arg);
    }
    contentItem ()->setHeight (arg);
}

void MyWindow::setActualWidth(int arg)
{
    if (actualWidth ()!= arg) {
        QQuickWindow::setWidth (arg);
        emit actualWidthChanged(arg);
    }
}

void MyWindow::setActualHeight(int arg)
{
    if (actualHeight ()!= arg) {
        QQuickWindow::setHeight (arg);
        emit actualHeightChanged(arg);
    }
}

void MyWindow::setShortcuts(MyWindowShortcutList *arg)
{
    if (m_shortcuts != arg) {
        m_shortcuts = arg;
        emit shortcutsChanged(arg);
    }
}


MyWindowShortcut::MyWindowShortcut(MyWindowShortcutList* parent):
    QObject(parent)
{
    setObjectName ("MyWindowShortcut");
    shortcutMapId = -1;
    m_enabled = true;
}

void MyWindowShortcut::setShortcut(QString arg)
{
    if (m_shortcut != arg) {
        m_shortcut = arg;
        key_list.clear ();
        QStringList list = arg.split ("+");
        foreach (QString key, list) {
            if(key!=""){
                if(key=="Ctrl"){
                    key_list.append (Qt::Key_Control);
                }else if(key=="Shift"){
                    key_list.append (Qt::Key_Shift);
                }else if(key=="Alt"){
                    key_list.append (Qt::Key_Alt);
                }else if(key=="Meta"){
                    key_list.append (Qt::Key_Meta);
                }else{
                    QKeySequence sequence=QKeySequence::fromString (key);
                    if(sequence!=Qt::Key_unknown){
                        key_list.append (sequence[0]);
                    }else{
                        emit shortcutError ("存在位置按键");
                        break;
                    }
                }
            }else{
                emit shortcutError ("不可有空按键");
                break;
            }
            qDebug()<<key<<key_list.last ();
        }
        emit shortcutChanged(arg);
    }
}

void MyWindowShortcut::setEnabled(bool arg)
{
    if (m_enabled != arg) {
        m_enabled = arg;
        emit enabledChanged(arg);
    }
}

void MyWindowShortcut::onKeyPressed(QQueue<int> &list)
{
    //qDebug()<<key_list<<list;
    if(list==key_list&&enabled())
        emit trigger ();
}

MyWindowShortcutList::MyWindowShortcutList(QQuickItem *parent):
    QQuickItem(parent)
{
}

void MyWindowShortcutList::componentComplete()
{
    foreach (QObject* temp, children ()) {
        if(temp->objectName ()=="MyWindowShortcut"){
            MyWindowShortcut* shortcut = qobject_cast<MyWindowShortcut*>(temp);
            mylist.append (shortcut);
        }
    }
}

QList<MyWindowShortcut *> &MyWindowShortcutList::list()
{
    return mylist;
}
