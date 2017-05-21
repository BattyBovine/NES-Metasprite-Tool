#include "spritelistwidget.h"

SpriteListWidget::SpriteListWidget(QWidget *parent) : QListWidget(parent)
{
	connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(updateListSelection()));
}

SpriteListWidget::~SpriteListWidget()
{
}



void SpriteListWidget::keyPressEvent(QKeyEvent *e)
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
	case Qt::Key_H:
		emit(this->flipSelectedHorizontal());
		break;
	case Qt::Key_V:
		emit(this->flipSelectedVertical());
		break;
	default:
		QListWidget::keyPressEvent(e);
	}
}



void SpriteListWidget::updateItems(GraphicsItemList l, GraphicsItemList sl)
{
	this->lItems.clear();
	foreach(QGraphicsItem *i, l) {
		if(i->type()!=MetaspriteTileItem::Type)   continue;
		this->lItems.append(i);
	}
	this->lSelectedItems = sl;
	this->generateListDisplay();
}

void SpriteListWidget::updateListSelection()
{
	foreach(QGraphicsItem *i, this->lItems) {
		i->setSelected(false);
	}
	foreach(QListWidgetItem *i, this->selectedItems()) {
		int index = this->row(i);
		this->lItems.at(index)->setSelected(true);
	}
}



void SpriteListWidget::generateListDisplay()
{
	this->clear();

	foreach(QGraphicsItem *i, this->lItems) {
		if(i->type()!=MetaspriteTileItem::Type)   continue;
		MetaspriteTileItem *ms = qgraphicsitem_cast<MetaspriteTileItem*>(i);
		QString lineitem = QString("%1 X=%2 Y=%3 P=%4")
						   .arg(ms->tileIndex(),2,16,QChar('0'))
						   .arg(ms->realX(),4,10,QChar(' '))
						   .arg(ms->realY(),4,10,QChar(' '))
						   .arg(ms->palette(),1,10,QChar(' ')).toUpper();
		lineitem += " ";
		if(ms->flippedHorizontal()) lineitem += "H";
		if(ms->flippedVertical()) lineitem += "V";
		this->addItem(lineitem);
	}

	foreach(QGraphicsItem *i, this->lSelectedItems) {
		if(i->type()!=MetaspriteTileItem::Type)   continue;
		this->item(this->lItems.indexOf(i))->setSelected(true);
	}
}
