#include "myshortcut.h"
#include <QEvent>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QAbstractEventDispatcher>

MyShortcut::MyShortcut(MyShortcut::Type type, QObject *parent):
    QObject(parent)
{
    m_shortcut = "";
    m_enabled = true;
    m_filterOut = true;
    m_shortcutType = type;
}

MyShortcut::MyShortcut(QString shortcut, MyShortcut::Type type, QObject *parent):
    QObject(parent)
{
    m_shortcut = shortcut;
    m_enabled = true;
    m_filterOut = true;
    m_shortcutType = type;
}

QString MyShortcut::shortcut() const
{
    return m_shortcut;
}

bool MyShortcut::isEnabled() const
{
    return m_enabled;
}

bool MyShortcut::filterOut() const
{
    return m_filterOut;
}

QObject *MyShortcut::target() const
{
    return m_target;
}

MyShortcut::Type MyShortcut::shortcutType() const
{
    return m_shortcutType;
}

void MyShortcut::setShortcut(QString arg)
{
    if (m_shortcut != arg) {
        m_shortcut = arg;
        if(shortcutType () == SystemGlobalShortcut){//如果是系统全局热键
            updataSystemGlobalShortcut (arg);//更新系统全局热键
        }
        
        key_list.clear ();
        QStringList list = arg.split ("+");
        foreach (QString key, list) {
            if(key!=""){
                if(QString::compare (key, "Ctrl", Qt::CaseInsensitive)==0){
                    key_list.append (Qt::Key_Control);
                }else if(QString::compare (key, "Shift", Qt::CaseInsensitive)==0){
                    key_list.append (Qt::Key_Shift);
                }else if(QString::compare (key, "Alt", Qt::CaseInsensitive)==0){
                    key_list.append (Qt::Key_Alt);
                }else if(QString::compare (key, "Meta", Qt::CaseInsensitive)==0){
                    key_list.append (Qt::Key_Meta);
                }else{
                    QKeySequence sequence=QKeySequence::fromString (key);
                    if(sequence!=Qt::Key_unknown){
                        key_list.append (sequence[0]);
                    }else{
                        emit error ("存在未知按键");
                        break;
                    }
                }
            }else{
                emit error ("不可有空按键");
                break;
            }
            //qDebug()<<key<<key_list.last ();
        }
        emit shortcutChanged(arg);
    }
}

void MyShortcut::setEnabled(bool arg)
{
    if (m_enabled != arg) {
        m_enabled = arg;
        if(obj){//如果对象不为空
            if(arg)
                obj->installEventFilter (this);//安装过滤器
            else
                obj->removeEventFilter (this);//移除过滤器
        }
        if(shortcutType ()==SystemGlobalShortcut){//如果是全局的
            if(global_shortcut)
                global_shortcut->setEnabled (arg);
        }
        emit enabledChanged(arg);
    }
}

bool MyShortcut::onKeyPressed(QQueue<int> &list)
{
    if(list==key_list){//判断是否触发了热键
        emit trigger ();
        return true;
    }
    return false;
}

void MyShortcut::setFilterOut(bool arg)
{
    if (m_filterOut != arg) {
        m_filterOut = arg;
        emit filterOutChanged(arg);
    }
}

void MyShortcut::setTarget(QObject *arg)
{
    if (m_target != arg) {
        m_target = arg;
        if(shortcutType ()==LocalShortcut)//如果是局部热键才能设置obj
            setObj (arg);
        emit targetChanged(arg);
    }
}

void MyShortcut::setShortcutType(MyShortcut::Type arg)
{
    if (m_shortcutType != arg) {
        m_shortcutType = arg;
        if(arg == AppGlobalShortcut){//如果是程序内部的全局热键
            //qDebug()<<"设置程序内部全局热键："<<QCoreApplication::instance ();
            setObj (QCoreApplication::instance ());//设置过滤对象
            //setObj (QAbstractEventDispatcher::instance ());//设置过滤对象
        }else if(arg == LocalShortcut){
            setObj (target ());//设置过滤对象
        }else if(arg == SystemGlobalShortcut){
            if(obj)
                obj->removeEventFilter (this);//移除过滤器
            if(global_shortcut.isNull ()){
                global_shortcut = new QxtGlobalShortcut(this);
                connect(global_shortcut, SIGNAL(activated()), SIGNAL(trigger()));//连接信号和槽
            }
            updataSystemGlobalShortcut(shortcut ());
        }
        emit shortcutTypeChanged(arg);
    }
}

void MyShortcut::setObj(QObject *arg)
{
    if(obj!=arg){
        if(isEnabled()&&arg){
            arg->installEventFilter (this);//为他安装事件过滤器
            //qDebug()<<"安装了过滤器";
        }
        if(obj)
            obj->removeEventFilter (this);//移除原来的过滤器
        obj = arg;//设置为新的过滤对象
    }
}

void MyShortcut::updataSystemGlobalShortcut(const QString &arg)
{
    if(arg=="")//不可为空
        return;
    QKeySequence temp = QKeySequence(arg);
    if(global_shortcut&&global_shortcut->shortcut ()!=temp){
        if(!global_shortcut->setShortcut (temp)){
            qDebug()<<"MyShortcut"<<"设置系统全局热键："+arg+"出错";
            emit error ("设置系统全局热键："+arg+"出错");
        }
    }
}

bool MyShortcut::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == obj||shortcutType () == AppGlobalShortcut){//判断一下是不是obj的事件，或者是全局热键事件
        if(event->type () == QEvent::KeyPress){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            int key = keyEvent->key ();
            if(queue_key.count ()==0||key!=queue_key.last ())//如果按键不是重复的
                queue_key.append (key);
            else
                return false;
            //qDebug()<<keyEvent->key ();
            return (onKeyPressed (queue_key)&&m_filterOut);
            //如果符合自己的热键设定，就再判断是否过滤掉此次按键事件，是的话才返回true，否则就算符合了热键的设定也将此事件放行给target响应
        }else if(event->type () == QEvent::KeyRelease){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            queue_key.removeOne (keyEvent->key());
            return false;
        }
        return false;
    }else{//如果是其他obj的时间就返回给parent中的eventFilter处理
        return QObject::eventFilter (watched, event);
    }
}
