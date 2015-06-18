#ifndef GRAPHICSVIEWSLIDER_H
#define GRAPHICSVIEWSLIDER_H

#include <QWidget>
#include <QSlider>

class GraphicsViewSlider : public QSlider
{
	Q_OBJECT
public:
	GraphicsViewSlider(QWidget *parent = 0);
	~GraphicsViewSlider();

public slots:
	void nextValue(){this->setValue(this->value()+this->singleStep());}
	void prevValue(){this->setValue(this->value()-this->singleStep());}
};

#endif // GRAPHICSVIEWSLIDER_H
