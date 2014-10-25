#ifndef MYSHORTCUT_H
#define MYSHORTCUT_H
/*!
 * 使用须知：
 * 此类为 雨后星辰后来添加，封装了app内部全局热键（通过给QApplication安装事件过滤器）
 * 封装了指定对象热键（为此对象安装事件过滤器实现）
 * 最后封装了qxtglobalshortcut实现了系统全局热键，在这里感谢原作者的贡献
 * 另外，由于qxtglobalshortcut不支持qt5，所以我额外给他增加了一下代码
 * 总结，此类支持对象内热键（必须为QObject类型或继承QObject），程序全局热键，系统全局热键，支持qt5和qt4(未尝试，可以需要修改部分代码)
 * 请不要删除这段话，谢谢！ 
*/
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
    enum Type{
        LocalShortcut,//局部热键
        AppGlobalShortcut,//整个程序的热键
        SystemGlobalShortcut//系统全局热键
    };
    explicit MyShortcut(Type type = LocalShortcut, QObject *parent = 0);
    explicit MyShortcut(QString shortcut, Type type = LocalShortcut, QObject *parent = 0);
    QString shortcut() const;
    bool isEnabled() const;
    bool filterOut() const;
    QObject* target() const;
    
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
