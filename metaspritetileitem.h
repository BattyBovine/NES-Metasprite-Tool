#ifndef METASPRITETILEITEM_H
#define METASPRITETILEITEM_H

#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPen>
#include <QGraphicsSceneMouseEvent>

#include <QMessageBox>

class MetaspriteTileItem : public QGraphicsPixmapItem
{
public:
    MetaspriteTileItem(QImage,QGraphicsItem *parent = 0);

    enum { Type = UserType+1 };
    int type() const {return Type;}

    void flipHorizontal(bool);
    void flipVertical(bool);
    int realX(){return qRound(this->x()/this->scale());}
    int realY(){return qRound(this->y()/this->scale());}

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
    bool bHFlip,bVFlip;
};

#endif // METASPRITETILEITEM_H
