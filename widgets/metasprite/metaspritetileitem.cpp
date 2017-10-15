#include "metaspritetileitem.h"

MetaspriteTileItem::MetaspriteTileItem(QGraphicsItem *parent) : QGraphicsItem(parent)
{
	this->bHFlip = this->bVFlip = false;
	this->iTile = 0;
	this->iBank = 0;
	this->iPalette = 0;
	this->iX = this->iY = 0;
	this->iW = MSTI_TILEWIDTH;
	this->iH = MSTI_TILEWIDTH*(this->bTallSprite?2:1);
	this->bTallSprite = false;

	this->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
	this->setZValue(-100);
}

MetaspriteTileItem::MetaspriteTileItem(quint32 tile, quint16 bank, quint8 palette, bool tall, QGraphicsItem *parent) : QGraphicsItem(parent)
{
	this->bHFlip = this->bVFlip = false;
	this->iTile = tile;
	this->iBank = bank;
	this->iPalette = palette;
	this->bTallSprite = tall;
	this->iW = MSTI_TILEWIDTH;
	this->iH = MSTI_TILEWIDTH*(this->bTallSprite?2:1);

	this->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
	this->setZValue(-100);
}



void MetaspriteTileItem::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
	QPixmap tileset = TilesetCache::find(this->iBank,this->iPalette);
	QRectF tile0 = QRect(((this->iTile%16)*MSTI_TILEWIDTH)&(this->bTallSprite?0xFE:0xFF),((this->iTile>>4)*MSTI_TILEWIDTH),MSTI_TILEWIDTH,MSTI_TILEWIDTH);
	p->drawPixmap(QRectF(0,0,MSTI_TILEWIDTH,MSTI_TILEWIDTH),tileset,tile0);
	if(this->bTallSprite) {
		QRectF tile1 = QRect(((this->iTile%16)*MSTI_TILEWIDTH)+MSTI_TILEWIDTH,((this->iTile>>4)*MSTI_TILEWIDTH),MSTI_TILEWIDTH,MSTI_TILEWIDTH);
		p->drawPixmap(QRectF(0,MSTI_TILEWIDTH,MSTI_TILEWIDTH,MSTI_TILEWIDTH),tileset,tile1);
	}

	if(this->isSelected()) {
		QPen dashes(Qt::black,(1/this->scale()),Qt::DashLine);
		QVector<qreal> dp;
		dp << 2 << 2;
		dashes.setDashPattern(dp);

		p->setPen(QPen(Qt::white,(1/this->scale())));
		p->drawRect(QRectF(0,0,MSTI_TILEWIDTH,MSTI_TILEWIDTH*(this->bTallSprite?2:1)));
		p->setPen(dashes);
		p->drawRect(QRectF(0,0,MSTI_TILEWIDTH,MSTI_TILEWIDTH*(this->bTallSprite?2:1)));

	}
}



void MetaspriteTileItem::flipHorizontal(bool f)
{
	if(f==this->bHFlip)  return;
	this->bHFlip = f;
	qreal t = qRound((this->width()/2)*this->scale());
	this->setTransform(QTransform().translate(t,0).scale(-1,1).translate(-t,0),true);
}

void MetaspriteTileItem::flipVertical(bool f)
{
	if(f==this->bVFlip)  return;
	this->bVFlip = f;
	qreal t = qRound((this->height()/2)*this->scale());
	this->setTransform(QTransform().translate(0,t).scale(1,-1),true);
}
