#include "spritepaletteview.h"
#include "palettemanager.h"

SpritePaletteView::SpritePaletteView(QWidget *parent) : QGraphicsView(parent)
{
}

SpritePaletteView::~SpritePaletteView()
{
}



void SpritePaletteView::mousePressEvent(QMouseEvent *e)
{
    QPointF p = mapToScene(e->pos());
    emit(spritePaletteSelected(this->objectName(),qFloor(p.x()/16)));
}

void SpritePaletteView::setPaletteScene(QGraphicsScene *s)
{
    this->setScene(s);
}
