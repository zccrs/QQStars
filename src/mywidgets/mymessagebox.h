#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QMessageBox>
#include <QUrl>
#include <QPixmap>

class MyMessageBox : public QMessageBox
{
    Q_OBJECT
    Q_PROPERTY(QUrl styleSource READ styleSource WRITE setStyleSource NOTIFY styleSourceChanged)
    
public:
    explicit MyMessageBox(QWidget *parent = 0);
    QUrl styleSource() const;
    
private:
     void mousePressEvent(QMouseEvent * event);
     void mouseMoveEvent(QMouseEvent * event);
     void mouseReleaseEvent(QMouseEvent * event);

     QUrl m_styleSource;
     QPoint press_point;
     bool allow_move;
     
     QPixmap *background_pixmap;
signals:
    void styleSourceChanged(QUrl arg);
public slots:
    void setStyleSource(QUrl arg);
};

#endif // MYMESSAGEBOX_H
