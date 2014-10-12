#ifndef MYSHORTCUT_H
#define MYSHORTCUT_H

#include <QObject>
#include <QPointer>
#include <QQueue>
#include "qxtglobalshortcut.h"//系统全局热键

class MyShortcut : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString shortcut READ shortcut WRITE setShortcut NOTIFY shortcutChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool filterOut READ filterOut WRITE setFilterOut NOTIFY filterOutChanged)
    //是将事件传递给过滤的对象,对于系统全局热键不可用
    Q_PROPERTY(QObject* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(Type shortcutType READ shortcutType WRITE setShortcutType NOTIFY shortcutTypeChanged)
    Q_ENUMS(Type)
public:
    explicit MyShortcut(QObject *parent = 0);
    QString shortcut() const;
    bool isEnabled() const;
    bool filterOut() const;
    QObject* target() const;
    
    enum Type{
        LocalShortcut,//局部热键
        AppGlobalShortcut,//整个程序的热键
        SystemGlobalShortcut//系统全局热键
    };
    
    Type shortcutType() const;
public slots:
    void setShortcut(QString arg);
    void setEnabled(bool arg);
    void setFilterOut(bool arg);
    void setTarget(QObject* arg);
    void setShortcutType(Type arg);
signals:
    void shortcutChanged(QString arg);
    void trigger();
    void enabledChanged(bool arg);
    void error(QString arg);
    void filterOutChanged(bool arg);
    void targetChanged(QObject* arg);
    void shortcutTypeChanged(Type arg);
    
private:
     QString m_shortcut;
     QQueue<int> key_list;
     QQueue<int> queue_key;
     bool m_enabled;
     bool m_filterOut;
     QPointer<QObject> m_target;
     QPointer<QObject> obj;
     QPointer<QxtGlobalShortcut> global_shortcut;
     Type m_shortcutType;
     
     void setObj(QObject *arg);
     void updataSystemGlobalShortcut(const QString &arg);//更新系统全局热键
     bool eventFilter ( QObject * watched, QEvent * event );
     bool onKeyPressed(QQueue<int>& list);
     
};

#endif // MYSHORTCUT_H
