#include "mymessagebox.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QPaintEngine>

MyMessageBox::MyMessageBox(QWidget *parent) :
    QMessageBox(parent)
{
    //setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize (300, 200);
    setWindowFlags (windowFlags ()|Qt::FramelessWindowHint);
    //QImage image(":/images/menu_background.png");
    //background_pixmap = QPixmap::fromImage (image);
    //QLabel *label = new QLabel(this);
    //label->setPixmap (background_pixmap);
    //label->setGeometry (0,0,width(),height ());
    //label->setBackgroundRole ();
}

QUrl MyMessageBox::styleSource() const
{
    return m_styleSource;
}

void MyMessageBox::mousePressEvent(QMouseEvent *event)
{
    if(event->button () == Qt::LeftButton){
        press_point = event->pos ();
        allow_move = true;
        event->accept ();
    }else{
        QMessageBox::mousePressEvent (event);
    }
}

void MyMessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if(allow_move){
        move (pos()+event->pos ()-press_point);
        event->accept ();
    }else{
        QMessageBox::mouseMoveEvent (event);
    }
}

void MyMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button () == Qt::LeftButton){
        allow_move = false;
        event->accept ();
    }else{
        QMessageBox::mouseReleaseEvent (event);
    }
}

void MyMessageBox::setStyleSource(QUrl arg)
{
    if (m_styleSource != arg) {
        m_styleSource = arg;
        QFile file(arg.toLocalFile ());
        if(file.open (QIODevice::ReadOnly)){
            setStyleSheet (file.readAll ());
        }else{
            qDebug()<<"打开"+arg.toLocalFile ()+"失败；"<<file.errorString ();
        }
        file.close ();
        emit styleSourceChanged(arg);
    }
}
