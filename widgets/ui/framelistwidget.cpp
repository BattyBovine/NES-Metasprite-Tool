#include "framelistwidget.h"

FrameListWidget::FrameListWidget(QWidget *parent) : QListWidget(parent)
{
	connect(this,SIGNAL(currentRowChanged(int)),this,SLOT(sendSelectedFrame()));
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
		this->deleteSelected();
		break;
	case Qt::Key_PageUp:
		emit(this->moveUpSelected());
		break;
	case Qt::Key_PageDown:
		emit(this->moveDownSelected());
		break;
	default:
		QListWidget::keyPressEvent(e);
	}
}



void FrameListWidget::newFrameData(AnimationFrameList l, int s)
{
	this->aflFrames = l;
	this->generateListDisplay(s);
}

void FrameListWidget::frameNumberUpdate(int f)
{
	if(!this->aflFrames.isEmpty()) {
		this->aflFrames[this->currentRow()].setFrame(f);
		emit(this->pushFrameUpdate(this->aflFrames));
		this->generateListDisplay(this->currentRow());
	}
}

void FrameListWidget::frameDelayUpdate(int d)
{
	if(!this->aflFrames.isEmpty()) {
		this->aflFrames[this->currentRow()].setDelay(d);
		emit(this->pushFrameUpdate(this->aflFrames));
		this->generateListDisplay(this->currentRow());
	}
}



void FrameListWidget::moveUpSelected()
{
	if(this->currentRow()>0) {
		AnimationFrameItem item = this->aflFrames.takeAt(this->currentRow());
		this->aflFrames.insert(this->currentRow()-1,item);

		this->generateListDisplay(this->currentRow()-1);
	}
}

void FrameListWidget::moveDownSelected()
{
	if(this->currentRow()<this->aflFrames.size()-1) {
		AnimationFrameItem item = this->aflFrames.takeAt(this->currentRow());
		this->aflFrames.insert(this->currentRow()+1,item);

		this->generateListDisplay(this->currentRow()+1);
	}
}

void FrameListWidget::addFrame()
{
	AnimationFrameItem frame(FLW_DEFAULT_FRAME,FLW_DEFAULT_DELAY);
	this->aflFrames.append(frame);

	this->generateListDisplay(this->aflFrames.size()-1);
}

void FrameListWidget::insertFrame()
{
	if(this->aflFrames.isEmpty()) {
		this->addFrame();
		return;
	}
	AnimationFrameItem frame(FLW_DEFAULT_FRAME,FLW_DEFAULT_DELAY);
	this->aflFrames.insert(this->currentRow(),frame);

	this->generateListDisplay(this->currentRow());
}

void FrameListWidget::deleteSelected()
{
	if(!this->aflFrames.isEmpty() && this->currentRow()<this->aflFrames.size()) {
		this->aflFrames.remove(this->currentRow());

		this->generateListDisplay((this->currentRow()>=this->aflFrames.size())?this->aflFrames.size()-1:this->currentRow());
	}
}



void FrameListWidget::sendSelectedFrame()
{
	if(this->currentRow()>=0 && this->currentRow()<this->aflFrames.size()) {
		emit(this->pushFrameUpdate(this->aflFrames));
		emit(this->selectedFrameChanged(this->currentRow()));
		emit(this->selectedFrameNumberChanged(this->aflFrames[this->currentRow()].frame()));
		emit(this->selectedFrameDelayChanged(this->aflFrames[this->currentRow()].delay()));
	}
}



void FrameListWidget::generateListDisplay(int s)
{
	this->clear();
	if(!this->aflFrames.isEmpty()) {
		for(int i=0; i<this->aflFrames.size(); i++) {
			this->addItem(this->generateListItem(i));
		}
	}
	this->setCurrentRow(s);
}

QString FrameListWidget::generateListItem(int i)
{
	return QString("%1").arg(i,3,10,QChar('0'))+
			QString(": MS=%1").arg(this->aflFrames[i].frame(),3,10,QChar('0'))+
			QString(",Delay=")+QString("%1").arg(this->aflFrames[i].delay(),3,10,QChar('0'));
}
