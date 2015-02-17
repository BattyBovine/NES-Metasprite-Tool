#ifndef SPRITELISTWIDGET_H
#define SPRITELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>

#include "common.h"

class SpriteListWidget : public QListWidget
{
    Q_OBJECT
public:
    SpriteListWidget(QWidget *parent = 0);
    ~SpriteListWidget();

signals:
    void moveSelectedUp();
    void moveSelectedDown();
    void deleteSelectedItems();

public slots:
    void updateItems(GraphicsItemList,GraphicsItemList);

private slots:
    void updateListSelection();

protected:
    void keyPressEvent(QKeyEvent*);

private:
    void generateListDisplay();

    QList<QGraphicsItem*> lItems;
    QList<QGraphicsItem*> lSelectedItems;
};

#endif // SPRITELISTWIDGET_H
