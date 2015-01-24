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

    if(e->button()==Qt::MiddleButton)
        QMessageBox::information(this,"Position",QString::number(sel.at(0)->x()/sel.at(0)->scale())+","+QString::number(sel.at(0)->y()/sel.at(0)->scale()),QMessageBox::NoButton);

    QGraphicsView::mouseReleaseEvent(e);
}

void MetaspriteManager::keyPressEvent(QKeyEvent *e)
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    switch(e->key()) {
    case Qt::Key_Delete:
        this->deleteSelectedTiles();
        break;

    case Qt::Key_Up:
    case Qt::Key_Down:
        foreach(QGraphicsItem *i, sel) {
            i->moveBy(0,(e->key()==Qt::Key_Down)?MSM_SCALE:(-MSM_SCALE));
        }
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        foreach(QGraphicsItem *i, sel) {
            i->moveBy((e->key()==Qt::Key_Right)?MSM_SCALE:(-MSM_SCALE),0);
        }
        break;

    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
        emit(requestPaletteUpdate(e->key()-Qt::Key_1));
        break;
    }
}



void MetaspriteManager::drawGridLines()
{
    QPen thicksolid(Qt::black,0.5,Qt::SolidLine);
    QPen thickdashes(Qt::white,0.5,Qt::DashLine);
    QPen thinsolid(Qt::black,0.25,Qt::SolidLine);
    QPen thindashes(Qt::white,0.25,Qt::DashLine);
    QVector<qreal> dp;
    dp << 1 << 1;
    thickdashes.setDashPattern(dp);
    thindashes.setDashPattern(dp);

    for(int i=MSM_TILESIZE; i<=128; i+=MSM_TILESIZE) {
        this->gsMetasprite->addLine(-128,-i,128,-i,thinsolid);
        this->gsMetasprite->addLine(-128,i,128,i,thinsolid);
        this->gsMetasprite->addLine(-i,-128,-i,128,thinsolid);
        this->gsMetasprite->addLine(i,-128,i,128,thinsolid);

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

void MetaspriteManager::setNewSpriteColours(QVector<QRgb> c, quint8 p)
{
    this->gsMetasprite->setBackgroundBrush(QBrush(QColor(c.at(0))));

    QList<QGraphicsItem*> items = this->gsMetasprite->items();
    foreach(QGraphicsItem *ms, items) {
        if(ms->type()!=MetaspriteTileItem::Type)   continue;
        for(int i=0; i<4; i++) {
            if(qgraphicsitem_cast<MetaspriteTileItem*>(ms)->palette()==i)
                qgraphicsitem_cast<MetaspriteTileItem*>(ms)->setNewColours(c.at((4*i)+1),c.at((4*i)+2),c.at((4*i)+3),i);
        }
    }

    items = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *i, items) {
        ((MetaspriteTileItem*)i)->setNewColours(c.at((4*p)+1),
                                                c.at((4*p)+2),
                                                c.at((4*p)+3),
                                                p);
    }
}

void MetaspriteManager::addTile(QPointF p, QImage t, quint8 c)
{
    MetaspriteTileItem *pi = new MetaspriteTileItem(t);
    pi->setFlags(QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsSelectable);
    pi->setScale(MSM_SCALE);
    pi->setPos((qRound(p.x()/MSM_SCALE)*MSM_SCALE),(qRound(p.y()/MSM_SCALE)*MSM_SCALE));
    pi->setPalette(c);
    this->gsMetasprite->addItem(pi);
}

void MetaspriteManager::deleteSelectedTiles()
{
    QList<QGraphicsItem*> sel = this->gsMetasprite->selectedItems();
    foreach(QGraphicsItem *s, sel) {
        this->gsMetasprite->removeItem(s);
    }
}

void MetaspriteManager::flipHorizontal()
{
    QList<QGraphicsItem*> sel = QList<QGraphicsItem*>(this->gsMetasprite->selectedItems());
    foreach(QGraphicsItem *s, sel) {
        ((MetaspriteTileItem*)s)->flipHorizontal(!((MetaspriteTileItem*)s)->flippedHorizontal());
    }
}

void MetaspriteManager::flipVertical()
{
    QList<QGraphicsItem*> sel = QList<QGraphicsItem*>(this->gsMetasprite->selectedItems());
    foreach(QGraphicsItem *s, sel) {
        ((MetaspriteTileItem*)s)->flipVertical(!((MetaspriteTileItem*)s)->flippedVertical());
    }
}
