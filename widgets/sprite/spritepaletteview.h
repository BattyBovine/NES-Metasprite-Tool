#ifndef SPRITEPALETTEVIEW_H
#define SPRITEPALETTEVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>

#include <QtMath>

#define SPV_SWATCH_SIZE 16

class SpritePaletteView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit SpritePaletteView(QWidget *parent = 0);
	~SpritePaletteView();

signals:
	void spritePaletteSelected(QString,quint8);

protected:
	void mousePressEvent(QMouseEvent*);

public slots:
	void setPaletteScene(QGraphicsScene*);
};

#endif // SPRITEPALETTEVIEW_H
