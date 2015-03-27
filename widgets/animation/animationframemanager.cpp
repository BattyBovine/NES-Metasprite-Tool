#include "animationframemanager.h"

AnimationFrameManager::AnimationFrameManager(QWidget *parent) : QGraphicsView(parent)
{
    this->gsFrame = new QGraphicsScene(this);
    this->setScene(this->gsFrame);
    this->iScale = AFM_DEFAULT_ZOOM;
    this->iFrame = 0;
    this->iDelay = 1;

    this->setSceneRect(-AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM,
                       -AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM,
                       AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM*2,
                       AFM_CANVAS_SIZE*AFM_DEFAULT_ZOOM*2);
    this->centerOn(0,0);
}

AnimationFrameManager::~AnimationFrameManager()
{
    this->gsFrame->clear();
    delete this->gsFrame;
}



void AnimationFrameManager::mousePressEvent(QMouseEvent *e)
{
    switch(e->button()) {
    case Qt::MiddleButton:
        this->iMouseTranslateX = e->x();
        this->iMouseTranslateY = e->y();
        break;
    default:
        QGraphicsView::mousePressEvent(e);
    }
}

void AnimationFrameManager::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);

    if(e->buttons()&Qt::MiddleButton) {
        this->setTransformationAnchor(QGraphicsView::NoAnchor);
        this->translate((e->x()-this->iMouseTranslateX),(e->y()-this->iMouseTranslateY));
        this->iMouseTranslateX = e->x();
        this->iMouseTranslateY = e->y();
    }
}

void AnimationFrameManager::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->buttons()&Qt::MiddleButton) {
        this->iScale = AFM_DEFAULT_ZOOM;
        this->centerOn(0,0);
        this->updateCurrentFrame();
    } else {
        QGraphicsView::mouseDoubleClickEvent(e);
    }
}

void AnimationFrameManager::wheelEvent(QWheelEvent *e)
{
    qreal steps = (((qreal)e->angleDelta().y()/8)/15)/4;
    if(((this->iScale+steps)>=1) && ((this->iScale+steps)<=AFM_MAX_ZOOM)) {
        this->iScale += steps;
    } else {
        this->iScale = ((steps<0)?1:AFM_MAX_ZOOM);
    }

    this->setSceneRect(-AFM_CANVAS_SIZE*this->iScale,
                       -AFM_CANVAS_SIZE*this->iScale,
                       AFM_CANVAS_SIZE*this->iScale*2,
                       AFM_CANVAS_SIZE*this->iScale*2);
    this->updateCurrentFrame();
}



void AnimationFrameManager::setBackgroundColour(PaletteVector c)
{
    this->gsFrame->setBackgroundBrush(QBrush(QColor(c.at(0))));
}

void AnimationFrameManager::updateCurrentFrame()
{
    emit(this->requestFrameData(this->iFrame,this->iScale));
}

void AnimationFrameManager::setNewFrame(int f)
{
    this->iFrame = f;
    emit(this->requestFrameData(this->iFrame,this->iScale));
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

void AnimationFrameManager::insertFrame()
{
    emit(this->insertAnimationFrame(this->iFrame, this->iDelay));
}

void AnimationFrameManager::replaceFrame()
{
    emit(this->replaceAnimationFrame(this->iFrame, this->iDelay));
}
