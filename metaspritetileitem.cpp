#include "metaspritetileitem.h"

MetaspriteTileItem::MetaspriteTileItem(QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->bHFlip = this->bVFlip = false;
    this->iPalette = 0;
}

MetaspriteTileItem::MetaspriteTileItem(QImage img, QGraphicsItem *parent) : QGraphicsPixmapItem(parent)
{
    this->bHFlip = this->bVFlip = false;
    this->iPalette = 0;
    this->imgTile = img;
    QGraphicsPixmapItem::setPixmap(QPixmap::fromImage(img));
}



void MetaspriteTileItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    QGraphicsPixmapItem::mouseReleaseEvent(e);
}

void MetaspriteTileItem::paint(QPainter *p, const QStyleOptionGraphicsItem *o, QWidget *w)
{
    Q_UNUSED(w);
    p->drawPixmap(QGraphicsPixmapItem::offset(),QGraphicsPixmapItem::pixmap());

    if(o->state & QStyle::State_Selected){
        QRectF r = this->pixmap().rect();
        QPen dashes(Qt::black,1/QGraphicsPixmapItem::scale(),Qt::DashLine);
        QVector<qreal> dp;
        dp << QGraphicsPixmapItem::scale() << QGraphicsPixmapItem::scale();
        dashes.setDashPattern(dp);
        p->setPen(QPen(Qt::white,1/QGraphicsPixmapItem::scale(),Qt::SolidLine));
        p->drawRect(r);
        p->setPen(dashes);
        p->drawRect(r);
    }
}



void MetaspriteTileItem::flipHorizontal(bool f)
{
    if(f==this->bHFlip)  return;
    this->bHFlip = f;
    qreal t = qRound(((this->boundingRect().width()-1)/2)*this->scale());
    this->setTransform(QTransform().translate(t,0).scale(-1,1).translate(-t,0),true);
}

void MetaspriteTileItem::flipVertical(bool f)
{
    if(f==this->bVFlip)  return;
    this->bVFlip = f;
    qreal t = qRound(((this->boundingRect().height()-1)/2)*this->scale());
    this->setTransform(QTransform().translate(0,t).scale(1,-1).translate(0,-t),true);
}



void MetaspriteTileItem::setTile(QImage img)
{
    this->imgTile = img;
    this->setPixmap(QPixmap::fromImage(this->imgTile));
}

void MetaspriteTileItem::setNewColours(QRgb a, QRgb b, QRgb c, quint8 p)
{
    this->iPalette = p;
    this->imgTile.setColor(1,a);
    this->imgTile.setColor(2,b);
    this->imgTile.setColor(3,c);
    this->setPixmap(QPixmap::fromImage(this->imgTile));
}
