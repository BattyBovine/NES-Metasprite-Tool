#ifndef METASPRITETILEITEM_H
#define METASPRITETILEITEM_H

#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

#include <QMessageBox>

#include <QtMath>

#include "tilesetcache.h"

#define MSTI_TILEWIDTH 8

class MetaspriteTileItem : public QGraphicsItem
{
public:
	MetaspriteTileItem(QGraphicsItem *parent = 0);
	MetaspriteTileItem(quint32,quint16,quint8,bool,QGraphicsItem *parent = 0);

	enum { Type = UserType+1 };
	int type() const {return Type;}

	void flipHorizontal(bool);
	void flipVertical(bool);
	int realX(){return qFloor(this->x()/this->scale());}
	int realY(){return qFloor(this->y()/this->scale());}
	void setRealX(qreal x){this->setX(x*this->scale());this->iX=x;}
	void setRealY(qreal y){this->setY(y*this->scale());this->iY=y;}
	qreal width(){return this->iW;}
	qreal height(){return this->iH*(this->bTallSprite?2:1);}
	bool tallSprite(){return this->bTallSprite;}
	void setTallSprite(bool dh){this->bTallSprite=dh;}

	void setTile(QImage);

	bool flippedHorizontal(){return this->bHFlip;}
	bool flippedVertical(){return this->bVFlip;}
	quint16 bank(){return this->iBank;}
	void setBank(quint16 b){this->iBank=b;}
	quint8 palette(){return this->iPalette;}
	void setPalette(quint8 p){this->iPalette=p;}
	quint32 tileIndex(){return this->iTile;}
	void setTileIndex(quint32 t){this->iTile=t;}

protected:
	QRectF boundingRect() const {return QRectF(0,0,MSTI_TILEWIDTH,(MSTI_TILEWIDTH*(this->bTallSprite?2:1)));}
	void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);

private:
	quint32 iTile;
	quint16 iBank;
	quint8 iPalette;
	qreal iX,iY,iW,iH;
	bool bTallSprite,bHFlip,bVFlip;
};

typedef QList<QGraphicsItem*> GraphicsItemList;
typedef QList<MetaspriteTileItem*> MetaspriteTileList;
typedef QVector<MetaspriteTileList> MetaspriteStageList;

#endif // METASPRITETILEITEM_H
