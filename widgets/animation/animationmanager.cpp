#include "animationmanager.h"

AnimationManager::AnimationManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsAnimation = new QGraphicsScene(this);
    this->setScene(this->gsAnimation);
    this->iAnimation = 0;
    this->iFrame = 0;

    this->alAnimations = AnimationList(256);
}

AnimationManager::~AnimationManager()
{
    this->gsAnimation->clear();
    delete this->gsAnimation;
}



void AnimationManager::addAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].append(frame);
    this->setAnimationFrame(this->alAnimations[this->iAnimation].size()-1);
    emit(this->framesUpdated(this->alAnimations[this->iAnimation]));
}

void AnimationManager::setBackgroundColour(PaletteVector c)
{
    this->gsAnimation->setBackgroundBrush(QBrush(QColor(c.at(0))));
}



void AnimationManager::setNewAnimation(int f)
{
    this->gsAnimation->clear();
    this->iAnimation = f;
    this->setAnimationFrame(0);
}

void AnimationManager::setAnimationFrame(int f)
{
    this->iFrame = f;
    if(!this->alAnimations[this->iAnimation].isEmpty() && this->iFrame < this->alAnimations[this->iAnimation].size()) {
        this->updateCurrentFrame();
    }
}

void AnimationManager::updateCurrentFrame()
{
    if(!this->alAnimations[this->iAnimation].isEmpty())
        emit(this->requestFrameData(this->alAnimations[this->iAnimation][this->iFrame].frame()));
}



void AnimationManager::drawAnimationFrameData(MetaspriteTileList f)
{
    this->gsAnimation->clear();
    foreach(MetaspriteTileItem *i, f) {
        this->gsAnimation->addItem(i);
    }
}
