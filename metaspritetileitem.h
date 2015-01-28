#ifndef METASPRITETILEITEM_H
#define METASPRITETILEITEM_H

#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

#include <QMessageBox>

#define MSTI_TILEWIDTH 8
#define MSTI_TILEHEIGHT MSTI_TILEWIDTH

class MetaspriteTileItem : public QGraphicsPixmapItem
{
public:
    MetaspriteTileItem(QGraphicsItem *parent = 0);
    MetaspriteTileItem(QImage,QGraphicsItem *parent = 0);

    enum { Type = UserType+1 };
    int type() const {return Type;}

    void flipHorizontal(bool);
    void flipVertical(bool);
    int realX(){return qRound(this->x()/this->scale());}
    int realY(){return qRound(this->y()/this->scale());}
    void setRealX(int x){this->setX(x*this->scale());this->iX=x;}
    void setRealY(int y){this->setY(y*this->scale());this->iY=y;}

    void setTile(QImage);
    void setNewColours(QRgb,QRgb,QRgb,quint8);

    bool flippedHorizontal(){return this->bHFlip;}
    bool flippedVertical(){return this->bVFlip;}
    quint8 palette(){return this->iPalette;}
    void setPalette(quint8 p){this->iPalette=p;}
    quint8 tile(){return this->iTile;}
    void setTile(quint8 t){this->iTile=t;}

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);

private:
    QImage imgTile;
    quint8 iPalette;
    quint8 iTile;
    int iX;
    int iY;
    bool bHFlip,bVFlip;
};

#endif // METASPRITETILEITEM_H
