#include "mymessagebox.h"
#include <QMouseEvent>
#include <QDebug>

MyMessageBox::MyMessageBox(QWidget *parent) :
    QMessageBox(parent)
{
    setFixedSize (300, 200);
    setWindowFlags (windowFlags ()|Qt::FramelessWindowHint);
    setAttribute ( Qt::WA_NoSystemBackground );
    update ();
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
