#include "framelistwidget.h"

FrameListWidget::FrameListWidget(QWidget *parent) : QListWidget(parent)
{
}

FrameListWidget::~FrameListWidget()
{
}



void FrameListWidget::keyPressEvent(QKeyEvent *e)
{
    QList<QListWidgetItem*> items = this->selectedItems();
    switch(e->key()) {
    case Qt::Key_Delete:
        foreach(QListWidgetItem* i, items) {
            this->removeItemWidget(i);
        }
        emit(this->deleteSelectedItems());
        break;
    case Qt::Key_PageUp:
        emit(this->moveSelectedUp());
        break;
    case Qt::Key_PageDown:
        emit(this->moveSelectedDown());
        break;
    default:
        QListWidget::keyPressEvent(e);
    }
}



void FrameListWidget::updateItems(AnimationFrameList l)
{
    this->lItems = l;
    this->generateListDisplay();
}



void FrameListWidget::generateListDisplay()
{
    this->clear();

    for(int i=0; i<this->lItems.size(); i++) {
//    foreach(AnimationFrameItem i, this->lItems) {
        QString lineitem = QString("%1").arg(i,3,10,QChar('0'))+
                QString(": MS=%1").arg(this->lItems[i].frame(),3,10,QChar('0'))+
                QString(",Delay=")+QString("%1").arg(this->lItems[i].delay(),3,10,QChar('0'));
        this->addItem(lineitem);
    }
}
