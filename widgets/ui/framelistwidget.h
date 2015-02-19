#ifndef FRAMELISTWIDGET_H
#define FRAMELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>

#include "animationframeitem.h"

class FrameListWidget : public QListWidget
{
    Q_OBJECT
public:
    FrameListWidget(QWidget *parent = 0);
    ~FrameListWidget();

signals:
    void moveSelectedUp();
    void moveSelectedDown();
    void deleteSelectedItems();

public slots:
    void updateItems(AnimationFrameList);

protected:
    void keyPressEvent(QKeyEvent*);

private:
    void generateListDisplay();

    AnimationFrameList lItems;
};

#endif // FRAMELISTWIDGET_H
