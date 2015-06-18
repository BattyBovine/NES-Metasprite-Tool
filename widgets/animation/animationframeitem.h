#ifndef ANIMATIONFRAMEITEM_H
#define ANIMATIONFRAMEITEM_H

#include "metaspritetileitem.h"

class AnimationFrameItem
{
public:
	AnimationFrameItem();
	AnimationFrameItem(quint8,quint8);

	quint8 frame(){return this->iFrame;}
	void setFrame(quint8 f){this->iFrame=f;}
	quint8 delay(){return this->iDelay;}
	void setDelay(quint8 d){this->iDelay=d;}

private:
	quint8 iFrame;
	quint8 iDelay;
};

typedef QVector<AnimationFrameItem> AnimationFrameList;

#endif // ANIMATIONFRAMEITEM_H
