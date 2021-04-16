#ifndef MY_LABEL_H
#define MY_LABEL_H

#include <QLabel>
#include <QWidget>
#include "widget.h"

class My_Label : public QLabel
{
    Q_OBJECT
public:
    My_Label(QWidget* parent=nullptr);
signals:
    void clicked();
private slots:
    void slotClicked();
protected:
    void mousePressEvent(QMouseEvent* e);
};

#endif // MY_LABEL_H
