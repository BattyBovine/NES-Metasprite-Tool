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
    void moveUp(int);
    void moveDown(int);
    void deleteItem(int);

public slots:
    void updateItems(AnimationFrameList,quint8);
    void moveUpSelected(){emit(this->moveUp(this->currentRow()));}
    void moveDownSelected(){emit(this->moveDown(this->currentRow()));}
    void deleteSelected(){emit(this->deleteItem(this->currentRow()));}

protected:
    void keyPressEvent(QKeyEvent*);

private:
    void generateListDisplay(quint8);

    AnimationFrameList lItems;
};

#endif // FRAMELISTWIDGET_H
