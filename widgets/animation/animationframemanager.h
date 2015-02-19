#ifndef ANIMATIONFRAMEMANAGER_H
#define ANIMATIONFRAMEMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include "common.h"
#include "animationframeitem.h"

class AnimationFrameManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AnimationFrameManager(QWidget *parent = 0);
    ~AnimationFrameManager();

signals:
    void requestFrameData(quint8);
    void addAnimationFrame(quint8,quint8);

public slots:
    void setBackgroundColour(PaletteVector);
    void updateCurrentFrame();
    void setNewFrame(int);
    void getFrameData(MetaspriteTileList);
    void setDelay(int d){this->iDelay=d;}

    void addFrame();

private:
    QGraphicsScene *gsFrame;
    quint8 iFrame;
    MetaspriteTileList mtlFrameData;
    quint8 iDelay;
};

#endif // ANIMATIONFRAMEMANAGER_H
