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
    void addAnimationFrame();
    void insertAnimationFrame();
    void replaceAnimationFrame(quint8);
    void replaceAnimationFrameDelay(quint8);
    void setSelectedFrame(int);
    void setSelectedDelay(int);

public slots:
    void updateItems(AnimationFrameList,quint8);
    void regenerateList(int);

    void moveUpSelected(){emit(this->moveUp(this->currentRow()));}
    void moveDownSelected(){emit(this->moveDown(this->currentRow()));}

    void addFrame(){emit(this->addAnimationFrame());}
    void insertFrame(){emit(this->insertAnimationFrame());}
    void replaceFrame(int f){emit(this->replaceAnimationFrame(f));}
    void replaceDelay(int d){emit(this->replaceAnimationFrameDelay(d));}
    void deleteSelected(){emit(this->deleteItem(this->currentRow()));}

protected:
    void keyPressEvent(QKeyEvent*);

private:
    void generateListDisplay(quint8);

    AnimationFrameList lItems;
    bool bUpdatingList;
};

#endif // FRAMELISTWIDGET_H
