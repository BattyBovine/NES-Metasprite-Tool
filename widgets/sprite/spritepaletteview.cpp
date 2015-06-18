#include "spritepaletteview.h"

SpritePaletteView::SpritePaletteView(QWidget *parent) : QGraphicsView(parent)
{
}

SpritePaletteView::~SpritePaletteView()
{
}



void SpritePaletteView::mousePressEvent(QMouseEvent *e)
{
	QPointF p = mapToScene(e->pos());
	if((p.x()<0||p.y()<0) || (p.x()>((0x04*SPV_SWATCH_SIZE)-1))||(p.y()>((0x01*SPV_SWATCH_SIZE)-1)))  return;
	emit(spritePaletteSelected(this->objectName(),qFloor(p.x()/SPV_SWATCH_SIZE)));
}

void SpritePaletteView::setPaletteScene(QGraphicsScene *s)
{
	s->setBackgroundBrush(QBrush(QColor(0x82,0x87,0x90)));
	this->setScene(s);
}
