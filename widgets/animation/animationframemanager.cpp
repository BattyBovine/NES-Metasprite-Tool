#include "animationframemanager.h"

AnimationFrameManager::AnimationFrameManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsFrame = new QGraphicsScene(this);
    this->setScene(this->gsFrame);
    this->iFrame = 0;
    this->iDelay = 1;
}

AnimationFrameManager::~AnimationFrameManager()
{
    this->gsFrame->clear();
    delete this->gsFrame;
}



void AnimationFrameManager::setBackgroundColour(PaletteVector c)
{
    this->gsFrame->setBackgroundBrush(QBrush(QColor(c.at(0))));
}

void AnimationFrameManager::updateCurrentFrame()
{
    emit(this->requestFrameData(this->iFrame));
}

void AnimationFrameManager::setNewFrame(int f)
{
    this->iFrame = f;
    emit(this->requestFrameData(this->iFrame));
}

void AnimationFrameManager::getFrameData(MetaspriteTileList l)
{
    this->mtlFrameData = l;
    this->gsFrame->clear();
    foreach(MetaspriteTileItem *i, this->mtlFrameData) {
        this->gsFrame->addItem(i);
    }
}



void AnimationFrameManager::addFrame()
{
    emit(this->addAnimationFrame(this->iFrame, this->iDelay));
}
