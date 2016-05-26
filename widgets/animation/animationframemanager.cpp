#include "animationframemanager.h"

AnimationFrameManager::AnimationFrameManager(QWidget *parent) : QGraphicsView(parent)
{
	this->gsFrame = new QGraphicsScene(this);
	this->setScene(this->gsFrame);
	this->iFrame = 0;

	this->iScale = AFM_DEFAULT_ZOOM;
	this->setSceneRect(-AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM,
					   -AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM,
					   AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM*2,
					   AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM*2);
	this->centerOn(0,0);
}

AnimationFrameManager::~AnimationFrameManager()
{
	this->gsFrame->clear();
	delete this->gsFrame;
}



void AnimationFrameManager::mousePressEvent(QMouseEvent *e)
{
	switch(e->button()) {
	case Qt::MiddleButton:
		this->iMouseTranslateX = e->x();
		this->iMouseTranslateY = e->y();
		break;
	default:
		QGraphicsView::mousePressEvent(e);
	}
}

void AnimationFrameManager::mouseMoveEvent(QMouseEvent *e)
{
	QGraphicsView::mouseMoveEvent(e);

	if(e->buttons()&Qt::MiddleButton) {
		this->setTransformationAnchor(QGraphicsView::NoAnchor);
		this->translate((e->x()-this->iMouseTranslateX),(e->y()-this->iMouseTranslateY));
		this->iMouseTranslateX = e->x();
		this->iMouseTranslateY = e->y();
	}
}

void AnimationFrameManager::mouseDoubleClickEvent(QMouseEvent *e)
{
	if(e->buttons()&Qt::MiddleButton) {
		this->iScale = AFM_DEFAULT_ZOOM;
		this->centerOn(0,0);
		this->updateCurrentFrame();
	} else {
		QGraphicsView::mouseDoubleClickEvent(e);
	}
}

void AnimationFrameManager::wheelEvent(QWheelEvent *e)
{
	qreal steps = (((qreal)e->angleDelta().y()/8)/15)/4;
	if(((this->iScale+steps)>=1) && ((this->iScale+steps)<=AFM_MAX_ZOOM)) {
		this->iScale += steps;
	} else {
		this->iScale = ((steps<0)?1:AFM_MAX_ZOOM);
	}

	this->setSceneRect(-AFM_CANVAS_SIZE*this->iScale,
					   -AFM_CANVAS_SIZE*this->iScale,
					   AFM_CANVAS_SIZE*this->iScale*2,
					   AFM_CANVAS_SIZE*this->iScale*2);
	this->updateCurrentFrame();
}



void AnimationFrameManager::setBackgroundColour(PaletteVector c)
{
	this->gsFrame->setBackgroundBrush(QBrush(QColor(c.at(0))));
}

void AnimationFrameManager::updateCurrentFrame()
{
	emit(this->requestFrameData(this->iFrame,this->iScale));
}

void AnimationFrameManager::setNewFrame(int f)
{
	this->iFrame = f;
	emit(this->requestFrameData(this->iFrame,this->iScale));
}

void AnimationFrameManager::getFrameData(MetaspriteTileList l)
{
	this->mtlFrameData = l;
	this->gsFrame->clear();
	this->getTileImageData(l);
	foreach(MetaspriteTileItem *i, this->mtlFrameData) {
		this->gsFrame->addItem(i);
	}
}

void AnimationFrameManager::getAnimationFrameData(MetaspriteTileList l)
{
	this->getTileImageData(l);
	emit(this->sendAnimationFrameData(l));
}

void AnimationFrameManager::getTilesetData(QImage tiles)
{
	this->imgTileset = tiles;
	this->getTileImageData(this->mtlFrameData);
}



MetaspriteTileList AnimationFrameManager::getTileImageData(MetaspriteTileList l)
{
	MetaspriteTileList newlist;
	foreach(MetaspriteTileItem *i, l) {
		QImage newtile(i->width(), i->height(), QImage::Format_Indexed8);
		newtile.setColor(0,i->getPaletteColour(0));
		newtile.setColor(1,i->getPaletteColour(1));
		newtile.setColor(2,i->getPaletteColour(2));
		newtile.setColor(3,i->getPaletteColour(3));

		int x = (i->tileIndex()&0x0000000F)*MSTI_TILEWIDTH;
		int y = ((i->tileIndex()&0xFFFFFFF0)>>4)*MSTI_TILEWIDTH;
		QImage toptile = (this->imgTileset.copy(x,y,MSTI_TILEWIDTH,MSTI_TILEWIDTH));

		for(quint8 y=0; y<MSTI_TILEWIDTH; y++) {
			for(quint8 x=0; x<MSTI_TILEWIDTH; x++) {
				newtile.setPixel(x,y,toptile.pixelIndex(x,y));
			}
		}

		if(newtile.height()>MSTI_TILEWIDTH) {
			x = ((i->tileIndex()+1)&0x0000000F)*MSTI_TILEWIDTH;
			y = ((i->tileIndex()&0xFFFFFFF0)>>4)*MSTI_TILEWIDTH;
			QImage antoniostellabottomtile = (this->imgTileset.copy(x,y,MSTI_TILEWIDTH,MSTI_TILEWIDTH));
			for(quint8 y=0; y<MSTI_TILEWIDTH; y++) {
				for(quint8 x=0; x<MSTI_TILEWIDTH; x++) {
					newtile.setPixel(x,y+MSTI_TILEWIDTH,antoniostellabottomtile.pixelIndex(x,y));
				}
			}
		}
		i->setTile(newtile);
		newlist.append(i);
	}
	return newlist;
}
