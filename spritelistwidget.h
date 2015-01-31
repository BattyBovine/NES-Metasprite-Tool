#ifndef SPRITELISTWIDGET_H
#define SPRITELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>

#include "metaspritetileitem.h"

class SpriteListWidget : public QListWidget
{
    Q_OBJECT
public:
    SpriteListWidget(QWidget *parent = 0);
    ~SpriteListWidget();

signals:
    void deleteSelectedItems();

public slots:
    void updateItems(QList<QGraphicsItem*>,QList<QGraphicsItem*>);

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
