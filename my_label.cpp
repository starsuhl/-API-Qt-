#include "my_label.h"

My_Label::My_Label(QWidget* parent):
    QLabel (parent)
{
    //connect(this,SIGNAL(clicked()),this,SLOT(slotClicked()));
}

void My_Label::slotClicked()
{

}

void My_Label::mousePressEvent(QMouseEvent *e)
{
    emit clicked();
}
