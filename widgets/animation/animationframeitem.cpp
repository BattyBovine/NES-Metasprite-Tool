#include "animationframeitem.h"

AnimationFrameItem::AnimationFrameItem()
{
	this->iFrame = 0;
	this->iDelay = 0;
}

AnimationFrameItem::AnimationFrameItem(quint8 f, quint8 d)
{
	this->iFrame = f;
	this->iDelay = d;
}
