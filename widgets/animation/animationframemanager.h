#ifndef ANIMATIONFRAMEMANAGER_H
#define ANIMATIONFRAMEMANAGER_H

#include <QWidget>
#include <QGraphicsView>

#include "common.h"

class AnimationFrameManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AnimationFrameManager(QWidget *parent = 0);
    ~AnimationFrameManager();

signals:
    void requestFrameData(int);

public slots:
    void setBackgroundColour(PaletteVector);
    void updateCurrentFrame();
    void setNewFrame(int);
    void getFrameData(MetaspriteTileList);

private:
    QGraphicsScene *gsFrame;
    quint8 iFrame;
};

#endif // ANIMATIONFRAMEMANAGER_H
