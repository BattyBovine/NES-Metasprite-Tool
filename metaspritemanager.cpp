#include "metaspritemanager.h"

MetaspriteManager::MetaspriteManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsMetasprite = new QGraphicsScene();
    this->setScene(this->gsMetasprite);

    this->drawGridLines();
}

MetaspriteManager::~MetaspriteManager()
{
    this->gsMetasprite->clear();
    delete this->gsMetasprite;
}



//void MetaspriteManager::dropEvent(QDropEvent *e)
//{
//    e->acceptProposedAction();

//    QMessageBox::information(this,"Dropped","Dropped.",QMessageBox::NoButton);
//}

void MetaspriteManager::mousePressEvent(QMouseEvent *e)
{
    if(e->button()!=Qt::RightButton)
        QGraphicsView::mousePressEvent(e);
    else
        emit(requestTile(this->mapToScene(e->pos())));
}

void MetaspriteManager::mouseReleaseEvent(QMouseEvent *e)
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *i, sel) {
        i->setPos((qRound(i->pos().x()/MSM_SCALE)*MSM_SCALE),(qRound(i->pos().y()/MSM_SCALE)*MSM_SCALE));
    }

    QGraphicsView::mouseReleaseEvent(e);
}



void MetaspriteManager::drawGridLines()
{
    QPen thicksolid(Qt::black,1,Qt::SolidLine);
    QPen thinsolid(Qt::black,0.25,Qt::SolidLine);
    QPen thickdashes(Qt::white,1,Qt::DashLine);
    QPen thindashes(Qt::white,0.25,Qt::DashLine);
    QVector<qreal> dp;
    dp << 1 << 1;
    thickdashes.setDashPattern(dp);
    thindashes.setDashPattern(dp);

    for(int i=0; i<=128; i+=8*MSM_SCALE) {
        this->gsMetasprite->addLine(-128,-i,128,-i,thinsolid);
        this->gsMetasprite->addLine(-128,i,128,i,thinsolid);
        this->gsMetasprite->addLine(-i,-128,-i,128,thinsolid);
        this->gsMetasprite->addLine(i,-128,i,128,thinsolid);
    }

    for(int i=0; i<=128; i+=8*MSM_SCALE) {
        this->gsMetasprite->addLine(-128,-i,128,-i,thindashes);
        this->gsMetasprite->addLine(-128,i,128,i,thindashes);
        this->gsMetasprite->addLine(-i,-128,-i,128,thindashes);
        this->gsMetasprite->addLine(i,-128,i,128,thindashes);
    }

    this->gsMetasprite->addLine(-128,0,128,0,thicksolid);
    this->gsMetasprite->addLine(0,-128,0,128,thicksolid);
    this->gsMetasprite->addLine(-128,0,128,0,thickdashes);
    this->gsMetasprite->addLine(0,-128,0,128,thickdashes);
}

void MetaspriteManager::setNewSpriteColours(QRgb c0)
{
    this->gsMetasprite->setBackgroundBrush(QBrush(QColor(c0)));
}

void MetaspriteManager::addTile(QPointF p, QPixmap t)
{
    QGraphicsPixmapItem *pi = new QGraphicsPixmapItem(t);
    pi->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
    pi->setPos(p);
    pi->setScale(MSM_SCALE);
    this->gsMetasprite->addItem(pi);
}
