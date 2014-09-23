#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QMessageBox>

class MyMessageBox : public QMessageBox
{
    Q_OBJECT
    
    QPoint press_point;
    bool allow_move;
    
public:
    explicit MyMessageBox(QWidget *parent = 0);
private:
     void mousePressEvent(QMouseEvent * event);
     void mouseMoveEvent(QMouseEvent * event);
     void mouseReleaseEvent(QMouseEvent * event);
signals:
    
public slots:
    
};

#endif // MYMESSAGEBOX_H
