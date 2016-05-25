#ifndef METASPRITETILEITEM_H
#define METASPRITETILEITEM_H

#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

#include <QMessageBox>

#include <QtMath>

#define MSTI_TILEWIDTH 8

class MetaspriteTileItem : public QGraphicsPixmapItem
{
public:
	MetaspriteTileItem(QGraphicsItem *parent = 0);
	MetaspriteTileItem(QImage,QGraphicsItem *parent = 0);
	MetaspriteTileItem(QImage,bool,QGraphicsItem *parent = 0);

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
	void setNewColours(QRgb,QRgb,QRgb,quint8);

	bool flippedHorizontal(){return this->bHFlip;}
	bool flippedVertical(){return this->bVFlip;}
	quint8 palette(){return this->iPalette;}
	void setPalette(quint8 p){this->iPalette=p;}
	quint32 tileIndex(){return this->iTile;}
	void setTileIndex(quint32 t){this->iTile=t;}
	QRgb getPaletteColour(quint8 i){return this->imgTile.color(i);}

protected:
	void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
	void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);

private:
	QImage imgTile;
	quint8 iPalette;
	quint32 iTile;
	qreal iX,iY,iW,iH;
	bool bTallSprite,bHFlip,bVFlip;
};

typedef QList<QGraphicsItem*> GraphicsItemList;
typedef QList<MetaspriteTileItem*> MetaspriteTileList;
typedef QVector<MetaspriteTileList> MetaspriteStageList;

#endif // METASPRITETILEITEM_H
