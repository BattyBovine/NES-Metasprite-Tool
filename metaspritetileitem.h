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

    void flipHorizontal(bool);
    void flipVertical(bool);
    bool flippedHorizontal();
    bool flippedVertical();

    void setNewColours(QRgb,QRgb,QRgb,quint8);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void paint(QPainter*,const QStyleOptionGraphicsItem*,QWidget*);

private:
    QImage imgTile;

    quint8 iPalette;
    bool bHFlip,bVFlip;
};

#endif // METASPRITETILEITEM_H
