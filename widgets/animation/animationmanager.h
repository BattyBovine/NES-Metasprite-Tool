#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QWidget>
#include <QGraphicsView>
#include <QItemSelection>

#include "common.h"
#include "animationframeitem.h"

class AnimationManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AnimationManager(QWidget *parent = 0);
    ~AnimationManager();

signals:
    void requestFrameData(quint8);
    void framesUpdated(AnimationFrameList);

public slots:
    void addAnimationFrame(quint8,quint8);
    void setBackgroundColour(PaletteVector);

    void setNewAnimation(int);
    void setAnimationFrame(int);
    void updateCurrentFrame();

    void drawAnimationFrameData(MetaspriteTileList);

private:
    QGraphicsScene *gsAnimation;
    quint8 iAnimation;
    AnimationList alAnimations;
    quint8 iFrame;

    void drawAnimationFrame(quint8);
};

#endif // ANIMATIONMANAGER_H
