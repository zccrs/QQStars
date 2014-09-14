#include "mysvgview.h"
#include <QPainter>
#include <QQuickWindow>
#include <QDebug>

MySvgView::MySvgView(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    svg = new QSvgRenderer(this);
    connect (this, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect (this, SIGNAL(heightChanged()), SLOT(onHeightChanged()));
}

void MySvgView::onWidthChanged()
{
    setImplicitHeight (svg->defaultSize ().height ()*(width ()/svg->defaultSize ().width ()));
}

void MySvgView::onHeightChanged()
{
    setImplicitWidth (svg->defaultSize ().width ()*(height ()/svg->defaultSize ().height ()));
}

void MySvgView::paint(QPainter *painter)
{
    svg->render (painter);
}

void MySvgView::setSource(QUrl arg)
{
    if (m_source != arg) {
        m_source = arg;
        QString str = arg.toString ();
        if( str.mid (0, 3) == "qrc")
            str = str.mid (3, str.count ()-3);
        svg->load (str);
        int temp1 = svg->defaultSize ().width ();
        int temp2 = svg->defaultSize ().height ();
        if(width ()==0&&height ()==0)
            setImplicitSize (temp1, temp2);//设置大小为默认大小
        else if(width()!=0)
            onWidthChanged();
        else if(height ()!=0)
            onHeightChanged ();
        update ();
        emit sourceChanged(arg);
    }
}
