#ifndef ANIMATIONFRAMEMANAGER_H
#define ANIMATIONFRAMEMANAGER_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

#include "palettemanager.h"
#include "animationframeitem.h"


#define AFM_CANVAS_SIZE     128
#define AFM_DEFAULT_ZOOM    2
#define AFM_MAX_ZOOM        8


class AnimationFrameManager : public QGraphicsView
{
    Q_OBJECT
public:
    explicit AnimationFrameManager(QWidget *parent = 0);
    ~AnimationFrameManager();

signals:
    void requestFrameData(quint8,qreal);

public slots:
    void setBackgroundColour(PaletteVector);
    void updateCurrentFrame();
    void setNewFrame(int);
    void getFrameData(MetaspriteTileList);

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent*);

private:
    QGraphicsScene *gsFrame;
    quint8 iFrame;
    MetaspriteTileList mtlFrameData;
    qreal iScale,iMouseTranslateX,iMouseTranslateY;
};

#endif // ANIMATIONFRAMEMANAGER_H
