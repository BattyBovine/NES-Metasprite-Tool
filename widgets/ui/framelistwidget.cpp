#include "framelistwidget.h"

FrameListWidget::FrameListWidget(QWidget *parent) : QListWidget(parent)
{
    //this->connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(regenerateList(int)));
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
        emit(this->deleteItem(this->currentRow()));
        break;
    case Qt::Key_PageUp:
        emit(this->moveUp(this->currentRow()));
        break;
    case Qt::Key_PageDown:
        emit(this->moveDown(this->currentRow()));
        break;
    default:
        QListWidget::keyPressEvent(e);
    }
}



void FrameListWidget::updateItems(AnimationFrameList l, quint8 s)
{
    this->lItems = l;
    this->generateListDisplay(s);
}

void FrameListWidget::regenerateList(int r)
{
    this->generateListDisplay(r);
}



void FrameListWidget::generateListDisplay(quint8 s)
{
    this->clear();

    if(!this->lItems.isEmpty()) {
        for(int i=0; i<this->lItems.size(); i++) {
            QString lineitem = QString("%1").arg(i,3,10,QChar('0'))+
                    QString(": MS=%1").arg(this->lItems[i].frame(),3,10,QChar('0'))+
                    QString(",Delay=")+QString("%1").arg(this->lItems[i].delay(),3,10,QChar('0'));
            this->addItem(lineitem);
        }

        this->setCurrentRow(s);

        emit(this->setSelectedFrame(this->lItems[s].frame()));
        emit(this->setSelectedDelay(this->lItems[s].delay()));
    }
}
