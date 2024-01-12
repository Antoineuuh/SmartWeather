#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <Includes.h>

class ClickableLabel : public QLabel {

    Q_OBJECT


public:
    explicit ClickableLabel(QWidget *parent = nullptr) : QLabel(parent){}

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *ev) {emit clicked();}


};



#endif // CLICKABLELABEL_H
