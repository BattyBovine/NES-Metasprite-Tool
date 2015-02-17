#include "animationframemanager.h"

AnimationFrameManager::AnimationFrameManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsFrame = new QGraphicsScene(this);
    this->gsFrame->setSceneRect(-127,-127,256,256);
    this->setScene(this->gsFrame);
    this->iFrame = 0;
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
    this->gsFrame->clear();
    foreach(MetaspriteTileItem *i, l) {
        this->gsFrame->addItem(i);
    }
}
