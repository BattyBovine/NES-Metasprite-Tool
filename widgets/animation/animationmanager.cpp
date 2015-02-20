#include "animationmanager.h"

AnimationManager::AnimationManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsAnimation = new QGraphicsScene(this);
    this->gsAnimation->setSceneRect(-128,-128,256,256);
    this->setScene(this->gsAnimation);
    this->iAnimation = 0;
    this->iSelectedFrame = this->iPlayingFrame = 0;
    this->iFrameTiming = AnimationManager::NTSC;
    this->isPlaying = false;

    this->alAnimations = AnimationList(256);

    connect(&this->tFrameCounter,SIGNAL(timeout()),this,SLOT(playNextFrame()));
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
    this->setSelectedFrame(this->alAnimations[this->iAnimation].size()-1);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::insertAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].insert(this->iSelectedFrame,frame);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::replaceAnimationFrame(quint8 f, quint8 d)
{
    AnimationFrameItem frame(f,d);
    this->alAnimations[this->iAnimation].replace(this->iSelectedFrame,frame);

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::setBackgroundColour(PaletteVector c)
{
    this->gsAnimation->setBackgroundBrush(QBrush(QColor(c.at(0))));
}



void AnimationManager::setNewAnimation(int f)
{
    this->tFrameCounter.stop();
    this->gsAnimation->clear();
    this->iAnimation = f;
    this->setSelectedFrame(0);
    emit(this->loopChanged(this->alAnimations[this->iAnimation].loop()));
    emit(this->labelChanged(this->alAnimations[this->iAnimation].label()));

    this->updateList(this->iSelectedFrame);
}

void AnimationManager::setSelectedFrame(int f)
{
    this->stopAnimation();
    this->iSelectedFrame = f;
    if(this->iSelectedFrame < this->alAnimations[this->iAnimation].size()) {
        this->updateCurrentFrame();
    }
}

void AnimationManager::setPlayingFrame(int f)
{
    this->iPlayingFrame = f;
    if(!this->alAnimations[this->iAnimation].isEmpty() && this->iPlayingFrame < this->alAnimations[this->iAnimation].size()) {
        emit(this->requestFrameData(this->alAnimations[this->iAnimation][this->iPlayingFrame].frame()));
    }
}

void AnimationManager::updateCurrentFrame()
{
    if(!this->alAnimations[this->iAnimation].isEmpty() && this->iSelectedFrame < this->alAnimations[this->iAnimation].size()) {
        emit(this->requestFrameData(this->alAnimations[this->iAnimation][this->iSelectedFrame].frame()));
    }
}

void AnimationManager::moveFrameUp(int i)
{
    if(i>0) {
        AnimationFrameItem item = this->alAnimations[this->iAnimation].takeAt(i);
        this->alAnimations[this->iAnimation].insert(--i,item);
        this->updateList(i);
    }
}

void AnimationManager::moveFrameDown(int i)
{
    if(i<this->alAnimations[this->iAnimation].size()-1) {
        AnimationFrameItem item = this->alAnimations[this->iAnimation].takeAt(i);
        this->alAnimations[this->iAnimation].insert(++i,item);
        this->updateList(i);
    }
}

void AnimationManager::deleteFrame(int i)
{
    if(i<this->alAnimations[this->iAnimation].size()) {
        this->alAnimations[this->iAnimation].remove(i);
        this->updateList((i==0)?i:i-1);
    }
}



void AnimationManager::playAnimationToggle(bool p)
{
    if(p) {
        this->playAnimation();
    } else {
        this->stopAnimation();
    }
}

void AnimationManager::playAnimation()
{
    if(!this->isPlaying) {
        this->isPlaying = true;
        if(this->alAnimations[this->iAnimation].isEmpty())  return;
        this->setPlayingFrame(0);
        this->playNextFrame(false);
        emit(this->animationStarted());
    }
}

void AnimationManager::playNextFrame(bool inc)
{
    if(inc) this->iPlayingFrame++;
    if(this->iPlayingFrame>=this->alAnimations[this->iAnimation].size()) {
        this->iPlayingFrame=0;
        if(!this->alAnimations[this->iAnimation].loop()) {
            this->stopAnimation();
            return;
        }
    }
    this->setPlayingFrame(this->iPlayingFrame);
    this->tFrameCounter.start(qRound((qreal(this->alAnimations[this->iAnimation][this->iPlayingFrame].delay())/qreal(this->iFrameTiming))*1000));
}

void AnimationManager::stopAnimation()
{
    if(this->isPlaying) {
        this->tFrameCounter.stop();
        emit(this->animationStopped());
        this->isPlaying = false;
    }
    this->setPlayingFrame(this->iSelectedFrame);
}



void AnimationManager::drawAnimationFrameData(MetaspriteTileList f)
{
    this->gsAnimation->clear();
    foreach(MetaspriteTileItem *i, f) {
        this->gsAnimation->addItem(i);
    }
}

void AnimationManager::updateList(quint8 s)
{
    this->gsAnimation->clear();
    emit(this->framesUpdated(this->alAnimations[this->iAnimation].frames(),s));
}
