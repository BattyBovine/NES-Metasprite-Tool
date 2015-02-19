#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QWidget>
#include <QGraphicsView>
#include <QTimer>

#include "common.h"
#include "animationframeitem.h"

class AnimationManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AnimationManager(QWidget *parent = 0);
    ~AnimationManager();

    enum {NTSC,PAL};

signals:
    void requestFrameData(quint8);
    void framesUpdated(AnimationFrameList,quint8);

public slots:
    void setNTSCTiming(){this->iFrameTiming=AnimationManager::NTSC;}
    void setPALTiming(){this->iFrameTiming=AnimationManager::PAL;}

    void addAnimationFrame(quint8,quint8);
    void setBackgroundColour(PaletteVector);

    void setNewAnimation(int);
    void setAnimationFrame(int);
    void updateCurrentFrame();

    void moveFrameUp(int);
    void moveFrameDown(int);
    void deleteFrame(int);

    void drawAnimationFrameData(MetaspriteTileList);

private:
    QGraphicsScene *gsAnimation;
    quint8 iAnimation;
    AnimationList alAnimations;
    quint8 iFrame;

    QTimer tFrameCounter;
    int iFrameTiming;
    bool bPlayAnimation;

    void drawAnimationFrame(quint8);
    void updateList(quint8);
};

#endif // ANIMATIONMANAGER_H
